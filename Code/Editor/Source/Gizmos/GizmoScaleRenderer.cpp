#include "brepch.h"
#include "GizmoScaleRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Core/Game.h"
#include "Bruno/Renderer/PrimitiveBatch.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

namespace Bruno
{
	GizmoScaleRenderer::GizmoScaleRenderer(GraphicsDevice* device, Camera& camera, std::shared_ptr<PrimitiveBatch<VertexPositionNormalColor>> batch, RenderConfig renderConfig) :
		m_camera(camera),
		m_batch(batch)
	{
		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_renderObjectBindingDesc.CBuffers[i] = std::make_shared<ConstantBuffer<GizmoObjectBuffer>>();
		}
		m_renderObjectBindingDesc.Space.SetCBV(m_renderObjectBindingDesc.CBuffers[0].get());
		m_renderObjectBindingDesc.Space.Lock();
		m_renderObjectBindingDesc.Shader = Graphics::GetShaderCache().Get(L"Shaders/UnlitColor.hlsl").get();
		m_renderObjectBindingDesc.Pipeline = Graphics::GetPsoCache().Get(RenderPsoId::UnlitColored).get();

		Math::Matrix world;
		world = Math::Matrix::CreateRotationZ(Math::ConvertToRadians(-90.0f)) * Math::Matrix::CreateTranslation(Math::Vector3::Right * (renderConfig.StickHeight + renderConfig.LineOffset) * 0.5f);
		CreateCylinder(renderConfig.StickRadius, renderConfig.StickHeight - renderConfig.LineOffset, renderConfig.Tessellation, renderConfig.StackCount, m_vertices, m_indices, Math::Vector4(1, 0, 0, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Right * (renderConfig.StickHeight - renderConfig.LineOffset + renderConfig.ArrowheadHeight) * 0.5f);
		CreateBox(Math::Vector3(renderConfig.ArrowheadHeight * 0.85f), m_vertices, m_indices, Math::Vector4(1, 0, 0, 1), world);

		m_xUpperBound = m_vertices.size();

		world = Math::Matrix::CreateTranslation(Math::Vector3::Up * (renderConfig.StickHeight + renderConfig.LineOffset) * 0.5f);
		CreateCylinder(renderConfig.StickRadius, renderConfig.StickHeight - renderConfig.LineOffset, renderConfig.Tessellation, renderConfig.StackCount, m_vertices, m_indices, Math::Vector4(0, 1, 0, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Up * (renderConfig.StickHeight - renderConfig.LineOffset + renderConfig.ArrowheadHeight * 0.85f) * 0.5f);
		CreateBox(Math::Vector3(renderConfig.ArrowheadHeight * 0.85f), m_vertices, m_indices, Math::Vector4(0, 1, 0, 1), world);

		m_yUpperBound = m_vertices.size();

		world = Math::Matrix::CreateRotationX(Math::ConvertToRadians(90.0f)) * Math::Matrix::CreateTranslation(Math::Vector3::Forward * (renderConfig.StickHeight + renderConfig.LineOffset) * 0.5f);
		CreateCylinder(renderConfig.StickRadius, renderConfig.StickHeight, renderConfig.Tessellation, 3, m_vertices, m_indices, Math::Vector4(0, 0, 1, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Forward * (renderConfig.StickHeight - renderConfig.LineOffset + renderConfig.ArrowheadHeight * 0.85f) * 0.5f);
		CreateBox(Math::Vector3(renderConfig.ArrowheadHeight * 0.85f), m_vertices, m_indices, Math::Vector4(0, 0, 1, 1), world);
	}

	void GizmoScaleRenderer::Render(GraphicsContext* context, Surface* surface)
	{
		Texture& backBuffer = surface->GetBackBuffer();
		DepthBuffer& depthBuffer = surface->GetDepthBuffer();

		PipelineInfo pipeline;
		pipeline.Pipeline = m_renderObjectBindingDesc.Pipeline;
		pipeline.RenderTargets.push_back(&backBuffer);
		pipeline.DepthStencilTarget = &depthBuffer;

		auto device = Graphics::GetDevice();
		m_renderObjectBindingDesc.Space.SetCBV(m_renderObjectBindingDesc.CBuffers[device->GetFrameId()].get());

		context->SetPipeline(pipeline);
		context->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_renderObjectBindingDesc.Space);

		m_batch->Begin(context);
		m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_indices.data(), m_indices.size(), m_vertices.data(), m_vertices.size());
		m_batch->End();
	}

	void GizmoScaleRenderer::SetColors(const Math::Color colors[3])
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

	void GizmoScaleRenderer::Update()
	{
		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_gizmoWorld * m_camera.GetViewProjection();

		GizmoObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_renderObjectBindingDesc.CBuffers[frameIndex]->SetMappedData(objectBuffer);
	}

	void GizmoScaleRenderer::CreateBox(const Math::Vector3& size, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
	{
		using namespace DirectX;

		// A box has six faces, each one pointing in a different direction.
		constexpr int FaceCount = 6;

		static const Math::Vector3 faceNormals[FaceCount] =
		{
			{Math::Vector3 {0, 0, 1} },
			{Math::Vector3 {0, 0, -1} },

			{Math::Vector3 {1, 0, 0} },
			{Math::Vector3 {-1, 0, 0} },

			{Math::Vector3 {0, 1,  0} },
			{Math::Vector3 {0, -1, 0} },
		};

		//static const XMVECTORF32 textureCoordinates[4] =
		//{
		//	{ { { 1, 0, 0, 0 } } },
		//	{ { { 1, 1, 0, 0 } } },
		//	{ { { 0, 1, 0, 0 } } },
		//	{ { { 0, 0, 0, 0 } } },
		//};

		XMVECTOR tsize = XMLoadFloat3(&size);
		tsize = XMVectorDivide(tsize, g_XMTwo);

		// Create each face in turn.
		for (int i = 0; i < FaceCount; i++)
		{
			const Math::Vector3 normal = Math::Vector3::TransformNormal(faceNormals[i], world);

			// Get two vectors perpendicular both to the face normal and to each other.
			XMVECTOR basis = (i >= 4) ? g_XMIdentityR2 : g_XMIdentityR1;
			basis=Math::Vector3::TransformNormal(basis, world);
			const XMVECTOR side1 = XMVector3Cross(normal, basis);
			const XMVECTOR side2 = XMVector3Cross(normal, side1);

			// Six indices (two triangles) per face.
			const size_t vbase = vertices.size();
			indices.push_back(vbase + 0);
			indices.push_back(vbase + 1);
			indices.push_back(vbase + 2);

			indices.push_back(vbase + 0);
			indices.push_back(vbase + 2);
			indices.push_back( vbase + 3);

			// Four vertices per face.
			// (normal - side1 - side2) * tsize // normal // t0

			Math::Vector3 pt = Math::Vector3::Transform(XMVectorMultiply(XMVectorSubtract(XMVectorSubtract(normal, side1), side2), tsize), world);
			vertices.push_back(VertexPositionNormalColor(pt, normal, color));

			// (normal - side1 + side2) * tsize // normal // t1
			pt = Math::Vector3::Transform(XMVectorMultiply(XMVectorAdd(XMVectorSubtract(normal, side1), side2), tsize), world);
			vertices.push_back(VertexPositionNormalColor(pt, normal, color));

			// (normal + side1 + side2) * tsize // normal // t2
			pt = Math::Vector3::Transform(XMVectorMultiply(XMVectorAdd(normal, XMVectorAdd(side1, side2)), tsize), world);
			vertices.push_back(VertexPositionNormalColor(pt, normal, color));

			// (normal + side1 - side2) * tsize // normal // t3
			pt = Math::Vector3::Transform(XMVectorMultiply(XMVectorSubtract(XMVectorAdd(normal, side1), side2), tsize), world);
			vertices.push_back(VertexPositionNormalColor(pt, normal, color));
		}
	}

	void GizmoScaleRenderer::CreateCylinder(float radius, float height, uint32_t sliceCount, uint32_t stackCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
	{
		// Create vertices
		float stackHeight = height / static_cast<float>(stackCount);
		float radiusStep = radius / static_cast<float>(stackCount);

		uint32_t shapeIndexOffset = vertices.size();
		for (uint32_t i = 0; i <= stackCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;

			for (uint32_t j = 0; j <= sliceCount; ++j)
			{
				float phi = DirectX::XM_2PI * static_cast<float>(j) / static_cast<float>(sliceCount);

				VertexPositionNormalColor vertex;
				vertex.Position.x = radius * std::cosf(phi);
				vertex.Position.y = y;
				vertex.Position.z = radius * std::sinf(phi);

				vertex.Position = Math::Vector3::Transform(vertex.Position, world);

				Math::Vector3 tangent(-std::sinf(phi), 0.0f, std::cosf(phi));
				Math::Vector3 bitangent(std::cosf(phi), 0.0f, std::sinf(phi));
				Math::Vector3 normal = tangent.Cross(bitangent);

				vertex.Normal = Math::Vector3::TransformNormal(normal, world);
				vertex.Normal.Normalize();
				//vertex.TexCoord.x = static_cast<float>(j) / static_cast<float>(sliceCount);
				//vertex.TexCoord.y = 1.0f - static_cast<float>(i) / static_cast<float>(stackCount);

				vertex.Color = color;
				vertices.push_back(vertex);
			}
		}
		uint32_t shapeVertexOffset = vertices.size();

		// Create indices
		for (uint32_t i = 0; i < stackCount; ++i)
		{
			for (uint32_t j = 0; j < sliceCount; ++j)
			{
				uint32_t baseIndex = shapeIndexOffset + i * (sliceCount + 1) + j;
				indices.push_back(baseIndex);
				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + sliceCount + 1);

				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + sliceCount + 2);
				indices.push_back(baseIndex + sliceCount + 1);
			}
		}

		// Create indices for the top face
		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			indices.push_back(shapeIndexOffset + (i + 1) % sliceCount + 1);  // Next top vertex
			indices.push_back(shapeIndexOffset + i + 1);              // Current top vertex
			indices.push_back(shapeIndexOffset);                  // Apex
		}

		// Calculate the baseVertex for the bottom face
		uint32_t baseVertex = static_cast<uint32_t>(shapeVertexOffset) - (sliceCount + 1);

		// Create indices for the bottom face
		for (uint32_t i = 0; i < sliceCount; ++i)
		{
			indices.push_back(baseVertex + i + 1);   // Current bottom vertex
			indices.push_back(baseVertex + (i + 1) % sliceCount + 1);   // Next bottom vertex
			indices.push_back(baseVertex);           // Center of the bottom face
		}
	}

	GizmoScaleRenderer::RenderConfig::RenderConfig(const GizmoConfig& gizmoConfig)
	{
		StickRadius = gizmoConfig.StickRadius;
		StickHeight = gizmoConfig.StickHeight;
		ArrowheadRadius = gizmoConfig.ArrowheadRadius;
		ArrowheadHeight = gizmoConfig.ArrowheadHeight;
		Tessellation = gizmoConfig.Tessellation;
		StackCount = gizmoConfig.StackCount;
		LineOffset = gizmoConfig.LineOffset;
	}
}