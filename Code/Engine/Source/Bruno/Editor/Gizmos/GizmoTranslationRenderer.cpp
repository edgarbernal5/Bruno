#include "brpch.h"
#include "GizmoTranslationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"

namespace Bruno
{
	GizmoTranslationRenderer::GizmoTranslationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface) :
		m_camera(camera),
		m_surface(surface)
	{
		m_batch = std::make_shared<PrimitiveBatch<VertexPositionNormalColor>>(device, 4096 * 3, 4096);

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_constantBuffers[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}

		m_shader = std::make_shared<Shader>(L"Shaders/UnlitColor.hlsl");

		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[0].get());
		m_meshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &m_meshPerObjectResourceSpace;

		PipelineResourceMapping resourceMapping;
		m_rootSignature = std::make_unique<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_shader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_shader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surface->GetDepthBufferFormat();
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RasterDesc.CullMode = D3D12_CULL_MODE_NONE;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		meshPipelineDesc.Topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		m_pipelineObject = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);
	}

	void GizmoTranslationRenderer::Render(GraphicsContext* context)
	{
		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_constantBuffers[frameIndex]->SetMappedData(objectBuffer);

		std::vector<VertexPositionNormalColor> vertices;
		std::vector<uint16_t> indices;
		CreateCylinder(2, 5, 20, 10, vertices, indices, Math::Vector4(1, 0, 0, 1));
		
		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[frameIndex].get());


		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_pipelineObject.get();
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;

		context->SetPipeline(pipeline);
		context->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

		m_batch->Begin(context);
		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indices.data(), indices.size(), vertices.data(), vertices.size());
		m_batch->End();
	}

	void GizmoTranslationRenderer::CreateCylinder(float radius, float height, uint32_t sliceCount, uint32_t stackCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color)
	{
		vertices.clear();
		indices.clear();

		// Create vertices
		float stackHeight = height / static_cast<float>(stackCount);
		float radiusStep = radius / static_cast<float>(stackCount);

		for (UINT i = 0; i <= stackCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = radius - i * radiusStep;

			for (UINT j = 0; j <= sliceCount; ++j)
			{
				float phi = DirectX::XM_2PI * static_cast<float>(j) / static_cast<float>(sliceCount);

				VertexPositionNormalColor vertex;
				vertex.Position.x = r * cosf(phi);
				vertex.Position.y = y;
				vertex.Position.z = r * sinf(phi);

				Math::Vector3 tangent(-std::sinf(phi), 0.0f, std::cosf(phi));
				Math::Vector3 bitangent(std::cosf(phi), 0.0f, std::sinf(phi));
				Math::Vector3 normal = tangent.Cross(bitangent);
				
				XMStoreFloat3(&vertex.Normal, normal);
				//vertex.TexCoord.x = static_cast<float>(j) / static_cast<float>(sliceCount);
				//vertex.TexCoord.y = 1.0f - static_cast<float>(i) / static_cast<float>(stackCount);

				vertex.Color = color;
				vertices.push_back(vertex);
			}
		}

		// Create indices
		for (UINT i = 0; i < stackCount; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				UINT baseIndex = i * (sliceCount + 1) + j;
				indices.push_back(baseIndex);
				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + sliceCount + 1);

				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + sliceCount + 2);
				indices.push_back(baseIndex + sliceCount + 1);
			}
		}
	}
}