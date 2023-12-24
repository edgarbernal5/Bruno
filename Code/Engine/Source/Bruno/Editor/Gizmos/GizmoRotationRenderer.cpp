#include "brpch.h"
#include "GizmoRotationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

namespace Bruno
{
	GizmoRotationRenderer::GizmoRotationRenderer(GraphicsDevice* device, Camera& camera, Surface* surface, std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> batch, RenderConfig renderConfig) :
		m_camera(camera),
		m_surface(surface),
		m_batch(batch),
		m_xUpperBound(0),
		m_yUpperBound(0),
		m_renderConfig(renderConfig)
	{
		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_renderObjectBindingDesc.CBuffers[i] = std::make_shared<ConstantBuffer<GizmoObjectBuffer>>();
		}
		m_renderObjectBindingDesc.Space.SetCBV(m_renderObjectBindingDesc.CBuffers[0].get());
		m_renderObjectBindingDesc.Space.Lock();
		m_renderObjectBindingDesc.Shader = Graphics::GetShaderCache().Get(L"Shaders/UnlitColor.hlsl").get();
		m_renderObjectBindingDesc.Pipeline = Graphics::GetPsoCache().Get(RenderPsoId::UnlitColored).get();
	}

	void GizmoRotationRenderer::Render(GraphicsContext* context)
	{
		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_renderObjectBindingDesc.Pipeline;
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;

		context->SetPipeline(pipeline);
		context->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_renderObjectBindingDesc.Space);

		m_batch->Begin(context);
		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_indices.data(), m_indices.size(), m_vertices.data(), m_vertices.size());
		m_batch->End();
	}

	void GizmoRotationRenderer::SetColors(const Math::Color colors[3])
	{
		m_axisColors[0] = colors[0];
		m_axisColors[1] = colors[1];
		m_axisColors[2] = colors[2];
	}

	void GizmoRotationRenderer::Update()
	{
		Math::Vector3 cameraToModelNormalized = m_gizmoWorld.Translation() - m_camera.GetPosition();
		cameraToModelNormalized.Normalize();
		auto worldInverse = m_gizmoWorld.Invert();
		cameraToModelNormalized = Math::Vector3::TransformNormal(cameraToModelNormalized, worldInverse);

		m_vertices.clear();
		m_indices.clear();

		Math::Matrix world;
		float angleStart;
		world = Math::Matrix::CreateRotationZ(Math::ConvertToRadians(-90.0f));
		angleStart = std::atan2f(cameraToModelNormalized.z, cameraToModelNormalized.y) - DirectX::XM_PI * 0.5f;
		CreateHalfTorus(m_renderConfig.Radius, m_renderConfig.Thickness, m_renderConfig.RingTessellation, angleStart, m_vertices, m_indices, m_axisColors[0], world);

		m_xUpperBound = m_vertices.size();

		world = world.Identity;
		angleStart = std::atan2f(cameraToModelNormalized.x, cameraToModelNormalized.z);
		CreateHalfTorus(m_renderConfig.Radius, m_renderConfig.Thickness, m_renderConfig.RingTessellation, angleStart, m_vertices, m_indices, m_axisColors[1], world);

		m_yUpperBound = m_vertices.size();

		world = Math::Matrix::CreateRotationX(Math::ConvertToRadians(90.0f));
		angleStart = std::atan2f(cameraToModelNormalized.y, cameraToModelNormalized.x) + DirectX::XM_PI * 0.5f;
		CreateHalfTorus(m_renderConfig.Radius, m_renderConfig.Thickness, m_renderConfig.RingTessellation, angleStart, m_vertices, m_indices, m_axisColors[2], world);

		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_gizmoWorld * m_camera.GetViewProjection();

		GizmoObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_renderObjectBindingDesc.CBuffers[frameIndex]->SetMappedData(objectBuffer);
	}

	void GizmoRotationRenderer::CreateHalfTorus(float radius, float thickness, size_t tessellation, float angleStart, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
	{
		using namespace DirectX;

		size_t verticesOffset = m_vertices.size();

		size_t halfTessellation = tessellation / 2;
		const size_t stride = halfTessellation + 1;
		// First we loop around the main ring of the torus.
		for (size_t i = 0; i <= halfTessellation; i++)
		{
			//const float u = float(i) / float(halfTessellation);
			const float outerAngle = angleStart + XM_PI * (float)i / halfTessellation;

			// Create a transform matrix that will align geometry to
			// slice perpendicularly though the current ring position.
			const XMMATRIX transform = XMMatrixTranslation(radius, 0, 0) * XMMatrixRotationY(outerAngle);

			// Now we loop along the other axis, around the side of the tube.
			for (size_t j = 0; j <= halfTessellation; j++)
			{
				//const float v = 1.0f - float(j) / float(tessellation);

				const float innerAngle = float(j) * XM_PI / float(halfTessellation);
				float dx, dy;

				XMScalarSinCos(&dy, &dx, innerAngle);

				// Create a vertex.
				XMVECTOR normal = XMVectorSet(dx, dy, 0, 0);
				XMVECTOR position = XMVectorScale(normal, thickness * 0.5f);
				//const XMVECTOR textureCoordinate = XMVectorSet(u, v, 0, 0);

				position = XMVector3Transform(position, transform);
				normal = XMVector3TransformNormal(normal, transform);
				Math::Vector3 positionV3 = Math::Vector3::Transform(position, world);
				Math::Vector3 normalV3 = normal;

				normalV3 = Math::Vector3::TransformNormal(normalV3, world);
				normalV3.Normalize();

				vertices.push_back(VertexPositionNormalColor(positionV3, normalV3, color));

				if (i <= halfTessellation - 1)
				{
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

	GizmoRotationRenderer::RenderConfig::RenderConfig(const GizmoConfig& gizmoConfig)
	{
		Radius = gizmoConfig.StickHeight + gizmoConfig.ArrowheadHeight;
		RingTessellation = gizmoConfig.RingTessellation;
		Thickness = gizmoConfig.RingThickness;
	}
}