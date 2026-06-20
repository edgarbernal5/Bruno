#include "brpch.h"
#include "Model.h"

#include "Material.h"
#include <Bruno/Platform/DirectX/VertexTypes.h>

#include "Bruno/Platform/DirectX/CommandQueueManager.h"
#include "Bruno/Platform/DirectX/UploadContext_Gem.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelMaterial);
	BR_RTTI_DEFINITIONS(Model);

	Model::Model(std::vector<ModelVertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<std::shared_ptr<Material>>&& materials, std::vector<std::shared_ptr<Mesh>>&& meshes, std::vector<ModelNode>&& modelNodes) :
		m_vertices(std::move(vertices)),
		m_indices(std::move(indices)),
		m_materials(std::move(materials)),
		m_meshes(std::move(meshes)),
		m_modelNodes(std::move(modelNodes))
	{
		//m_handle = {};
		auto device = Bruno::Graphics::GetDXDevice();

		std::vector< VertexPositionNormalTexture> verticesPNT;
		verticesPNT.reserve(m_vertices.size());
		for (size_t i = 0; i < m_vertices.size(); i++)
		{
			auto& vertex = verticesPNT.emplace_back();
			vertex.Position = m_vertices[i].Position;
			vertex.Texture = Math::Vector2(m_vertices[i].Texcoord.x, m_vertices[i].Texcoord.y);
			vertex.Normal = m_vertices[i].Normal;
		}
		
		auto& uploadContext = device->GetUploadContext();
		auto& commandManager = Graphics::GetCommandQueueManager();
		
		uploadContext.Reset();
		
		m_indexBuffer = std::make_shared<DX::IndexBuffer>(*device, uploadContext, m_indices);
		m_vertexBuffer = std::make_shared<DX::VertexBuffer>(*device, uploadContext, verticesPNT.data(), verticesPNT.size(), sizeof(VertexPositionNormalTexture));
		
		uint64_t uploadFence = commandManager->ExecuteAndReturnFence(uploadContext);

		commandManager->WaitForGpuFence(uploadFence);

		uploadContext.ClearGarbage();
	}

	void Model::SetIndexBuffer(std::shared_ptr<DX::IndexBuffer> buffer)
	{
		m_indexBuffer = buffer;
	}

	void Model::SetVertexBuffer(std::shared_ptr<DX::VertexBuffer> buffer)
	{
		m_vertexBuffer = buffer;
	}

	std::shared_ptr<Material>& Model::GetMaterial(uint32_t materialIndex)
	{
		return m_materials[materialIndex];
	}

	Mesh::Mesh(const std::wstring& meshName, uint32_t baseVertex, uint32_t baseIndex, uint32_t vertexCount, uint32_t indexCount, uint32_t materialIndex, 
		const Math::Matrix& transform, const Math::Matrix& localTransform, const Math::BoundingBox& bbox) :
		m_meshName(meshName),
		m_baseVertex(baseVertex),
		m_baseIndex(baseIndex),
		m_vertexCount(vertexCount),
		m_indexCount(indexCount),
		m_materialIndex(materialIndex),
		m_transform(transform),
		m_localTransform(localTransform),
		m_bBox(bbox)
	{
	}
}
