#include "brpch.h"
#include "Model.h"

#include "Material.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelMaterial);
	BR_RTTI_DEFINITIONS(Model);

	Model::Model(std::vector<ModelVertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<std::shared_ptr<ModelMaterial>>&& materials, std::vector<std::shared_ptr<Mesh>>&& meshes, std::vector<ModelNode>&& modelNodes) :
		m_vertices(std::move(vertices)),
		m_indices(std::move(indices)),
		m_materials(std::move(materials)),
		m_meshes(std::move(meshes)),
		m_modelNodes(std::move(modelNodes))
	{
		m_assetHandle = {};
	}

	void Model::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
	{
		m_indexBuffer = buffer;
	}

	void Model::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
	{
		m_vertexBuffer = buffer;
	}

	std::shared_ptr<ModelMaterial>& Model::GetMaterial(uint32_t materialIndex)
	{
		return m_materials[materialIndex];
	}

	Mesh::Mesh(const std::string& meshName, uint32_t baseVertex, uint32_t baseIndex, uint32_t vertexCount, uint32_t indexCount, uint32_t materialIndex, const Math::Matrix& transform, const Math::Matrix& localTransform, const Math::BoundingBox& bbox) :
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