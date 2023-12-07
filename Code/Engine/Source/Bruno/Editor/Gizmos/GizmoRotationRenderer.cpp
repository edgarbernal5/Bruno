#include "brpch.h"
#include "GizmoRotationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

namespace Bruno
{
	GizmoRotationRenderer::GizmoRotationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface, std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> batch) :
		m_camera(camera),
		m_surface(surface),
		m_batch(batch)
	{
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
		
		Math::Matrix world;
		world = Math::Matrix::CreateRotationZ(Math::ConvertToRadians(90.0f));
		CreateTorus(Gizmo::GIZMO_LENGTH, Gizmo::CONE_RADIUS * 0.5f, Gizmo::RING_TESSELLATION, m_vertices, m_indices, Math::Color(1, 0, 0, 1), world);

		m_xUpperBound = m_vertices.size();

		world = world.Identity;
		CreateTorus(Gizmo::GIZMO_LENGTH, Gizmo::CONE_RADIUS * 0.5f, Gizmo::RING_TESSELLATION, m_vertices, m_indices, Math::Color(0, 1, 0, 1), world);
		
		m_yUpperBound = m_vertices.size();

		world = Math::Matrix::CreateRotationX(Math::ConvertToRadians(90.0f));
		CreateTorus(Gizmo::GIZMO_LENGTH, Gizmo::CONE_RADIUS * 0.5f, Gizmo::RING_TESSELLATION, m_vertices, m_indices, Math::Color(0, 0, 1, 1), world);
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
		for (size_t i = 0; i < m_xUpperBound; i++)
		{
			m_vertices[i].Color = colors[0];
		}
		for (size_t i = m_xUpperBound; i < m_yUpperBound; i++)
		{
			m_vertices[i].Color = colors[1];
		}
		for (size_t i = m_yUpperBound; i < m_vertices.size(); i++)
		{
			m_vertices[i].Color = colors[2];
		}
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

		int verticesOffset = m_vertices.size();

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
				Math::Vector3 normalV3 = normal;

				normalV3 = Math::Vector3::TransformNormal(normalV3, world);
				normalV3.Normalize();

				vertices.push_back(VertexPositionNormalColor(positionV3, normalV3, color));

				// And create indices for two triangles.
				const size_t nextI = (i + 1) % stride;
				const size_t nextJ = (j + 1) % stride;

				indices.push_back(verticesOffset + i * stride + j);
				indices.push_back(verticesOffset + i * stride + nextJ);
				indices.push_back(verticesOffset + nextI * stride + j);

				indices.push_back(verticesOffset + i * stride + nextJ);
				indices.push_back(verticesOffset + nextI * stride + nextJ);
				indices.push_back(verticesOffset + nextI * stride + j);
			}
		}
	}
}