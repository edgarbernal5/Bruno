#include "brpch.h"
#include "GizmoTranslationRenderer.h"

#include "Bruno/Platform/DirectX/GraphicsContext.h"
#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Renderer/Camera.h"
#include "GizmoService.h"

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
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surface->GetSurfaceFormat();
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineObject = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);

		Math::Matrix world;
		world = Math::Matrix::CreateRotationZ(Math::ConvertToRadians(-90.0f)) * Math::Matrix::CreateTranslation(Math::Vector3::Right * Gizmo::LINE_LENGTH * 0.5f);
		CreateCylinder(Gizmo::CONE_RADIUS * 0.25f, Gizmo::LINE_LENGTH, 4, 3, m_vertices, m_indices, Math::Vector4(1, 0, 0, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Right * (Gizmo::LINE_LENGTH) * 0.5f);
		CreateCone(Gizmo::CONE_RADIUS, Gizmo::CONE_HEIGHT, 4, m_vertices, m_indices, Math::Vector4(1, 0, 0, 1), world);

		m_xUpperBound = m_vertices.size();

		world = Math::Matrix::CreateTranslation(Math::Vector3::Up * Gizmo::LINE_LENGTH * 0.5f);
		CreateCylinder(Gizmo::CONE_RADIUS * 0.25f, Gizmo::LINE_LENGTH, 4, 3, m_vertices, m_indices, Math::Vector4(0, 1, 0, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Up * (Gizmo::CONE_HEIGHT + Gizmo::LINE_LENGTH) * 0.5f);
		CreateCone(Gizmo::CONE_RADIUS, Gizmo::CONE_HEIGHT, 4, m_vertices, m_indices, Math::Vector4(0, 1, 0, 1), world);

		m_yUpperBound = m_vertices.size();

		world = Math::Matrix::CreateRotationX(Math::ConvertToRadians(90.0f)) * Math::Matrix::CreateTranslation(Math::Vector3::Forward * Gizmo::LINE_LENGTH * 0.5f);
		CreateCylinder(Gizmo::CONE_RADIUS * 0.25f, Gizmo::LINE_LENGTH, 4, 3, m_vertices, m_indices, Math::Vector4(0, 0, 1, 1), world);
		world = world * Math::Matrix::CreateTranslation(Math::Vector3::Forward * (Gizmo::CONE_HEIGHT + Gizmo::LINE_LENGTH) * 0.5f);
		CreateCone(Gizmo::CONE_RADIUS, Gizmo::CONE_HEIGHT, 4, m_vertices, m_indices, Math::Vector4(0, 0, 1, 1), world);
	}

	void GizmoTranslationRenderer::Render(GraphicsContext* context)
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

	void GizmoTranslationRenderer::SetColors(const Math::Color colors[3])
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

	void GizmoTranslationRenderer::Update()
	{
		auto device = Graphics::GetDevice();
		Math::Matrix mvpMatrix = m_gizmoWorld * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_constantBuffers[frameIndex]->SetMappedData(objectBuffer);

		m_meshPerObjectResourceSpace.SetCBV(m_constantBuffers[frameIndex].get());
	}

	void GizmoTranslationRenderer::CreateCone(float radius, float height, uint32_t sliceCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
	{
		size_t verticesOffset = vertices.size();
		
		height *= 0.5f;

		Math::Vector3 topOffset(0, height, 0);

		const size_t stride = sliceCount + 1;

		// Create the base vertices
		for (uint32_t i = 0; i <= sliceCount; ++i)
		{
			float phi = 2.0f * DirectX::XM_PI * i / sliceCount;

			VertexPositionNormalColor vertex;
			Math::Vector3 pt = Math::Vector3(radius * std::sinf(phi), -height, radius * std::cosf(phi));
			
			float phi2 = phi + DirectX::XM_PIDIV2;
			Math::Vector3 tangent(std::sinf(phi2), 0.0f, std::cosf(phi2));
			Math::Vector3 bitangent = topOffset - pt;
			Math::Vector3 normal = tangent.Cross(bitangent);

			//vertex.TexCoord = DirectX::XMFLOAT2((cosf(phi) + 1.0f) * 0.5f, (sinf(phi) + 1.0f) * 0.5f);
			vertex.Color = color;

			vertex.Normal = normal;
			vertex.Position = Math::Vector3::Transform(topOffset, world);
			vertices.push_back(vertex);

			vertex.Position = Math::Vector3::Transform(pt, world);
			vertices.push_back(vertex);

			indices.push_back(verticesOffset + i * 2);
			indices.push_back(verticesOffset + (i * 2 + 3) % (stride * 2));
			indices.push_back(verticesOffset + (i * 2 + 1) % (stride * 2));
		}

		// Create cap indices.
		for (size_t i = 0; i < sliceCount - 2; i++)
		{
			size_t i1 = (i + 1) % sliceCount;
			size_t i2 = (i + 2) % sliceCount;

			const size_t vbase = vertices.size();
			indices.push_back(vbase);
			indices.push_back(vbase + i1);
			indices.push_back(vbase + i2);
		}

		Math::Vector3 normal = Math::Vector3::Down;
		// Create cap vertices.
		for (size_t i = 0; i < sliceCount; i++)
		{
			float phi = 2.0f * DirectX::XM_PI * i / sliceCount;

			VertexPositionNormalColor vertex;
			Math::Vector3 pt = Math::Vector3(radius * std::sinf(phi), normal.y * height, radius * std::cosf(phi));

			vertex.Position = pt;
			vertex.Normal = normal;
			vertex.Color = color;
			vertex.Position = Math::Vector3::Transform(vertex.Position, world);
			vertices.push_back(vertex);
		}
	}

	void GizmoTranslationRenderer::CreateCylinder(float radius, float height, uint32_t sliceCount, uint32_t stackCount, std::vector<VertexPositionNormalColor>& vertices, std::vector<uint16_t>& indices, const Math::Vector4& color, const Math::Matrix& world)
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
				//Math::Vector3 normal = bitangent.Cross(tangent);

				vertex.Normal = normal;
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
}