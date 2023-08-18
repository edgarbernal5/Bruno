#include "brpch.h"
#include "Model.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Model);

	Model::Model(std::vector<std::shared_ptr<Mesh>>&& meshes) :
		m_meshes(std::move(meshes))
	{
	}

	Mesh::Mesh(std::vector<Math::Vector3>&& vertices, std::vector<Math::Vector3>&& normals, std::vector<std::vector<Math::Vector3>>&& texCoords) :
		m_vertices(std::move(vertices)),
		m_normals(std::move(normals)),
		m_textureCoordinates(std::move(texCoords))
	{
	}

	void Mesh::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
	{
		m_indexBuffer = buffer;
	}

	void Mesh::SetMaterial(std::shared_ptr<Material> material)
	{
		m_material = material;
	}

	void Mesh::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
	{
		m_vertexBuffer = buffer;
	}
}