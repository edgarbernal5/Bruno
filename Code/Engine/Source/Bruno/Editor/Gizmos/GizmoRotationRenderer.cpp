#include "brpch.h"
#include "GizmoRotationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

namespace Bruno
{
	GizmoRotationRenderer::GizmoRotationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface) :
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
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineObject = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);


	}

	void GizmoRotationRenderer::Render(GraphicsContext* context)
	{
		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_pipelineObject.get();
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;

		context->SetPipeline(pipeline);
		context->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

		m_batch->Begin(context);
		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_indices.data(), m_indices.size(), m_vertices.data(), m_vertices.size());
		m_batch->End();
	}

	void GizmoRotationRenderer::SetColors(const Math::Color colors[3])
	{

	}

	void GizmoRotationRenderer::Update()
	{
		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_gizmoWorld * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_constantBuffers[frameIndex]->SetMappedData(objectBuffer);

		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[frameIndex].get());
	}

	void GizmoRotationRenderer::CreateTorus(float diameter, float thickness, size_t tessellation, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
	{
		using namespace DirectX;

		const size_t stride = tessellation + 1;
		// First we loop around the main ring of the torus.
		for (size_t i = 0; i <= tessellation; i++)
		{
			const float u = float(i) / float(tessellation);

			const float outerAngle = float(i) * XM_2PI / float(tessellation) - XM_PIDIV2;

			// Create a transform matrix that will align geometry to
			// slice perpendicularly though the current ring position.
			const XMMATRIX transform = XMMatrixTranslation(diameter / 2, 0, 0) * XMMatrixRotationY(outerAngle);

			// Now we loop along the other axis, around the side of the tube.
			for (size_t j = 0; j <= tessellation; j++)
			{
				const float v = 1 - float(j) / float(tessellation);

				const float innerAngle = float(j) * XM_2PI / float(tessellation) + XM_PI;
				float dx, dy;

				XMScalarSinCos(&dy, &dx, innerAngle);

				// Create a vertex.
				XMVECTOR normal = XMVectorSet(dx, dy, 0, 0);
				XMVECTOR position = XMVectorScale(normal, thickness / 2);
				//const XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

				position = XMVector3Transform(position, transform);
				normal = XMVector3TransformNormal(normal, transform);
				Math::Vector3 positionV3 = Math::Vector3::Transform(position, world);

				vertices.push_back(VertexPositionNormalColor(positionV3, Math::Vector3(normal), color));

				// And create indices for two triangles.
				const size_t nextI = (i + 1) % stride;
				const size_t nextJ = (j + 1) % stride;

				indices.push_back(i * stride + j);
				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + j);

				indices.push_back(i * stride + nextJ);
				indices.push_back(nextI * stride + nextJ);
				indices.push_back(nextI * stride + j);
			}
		}
	}
}