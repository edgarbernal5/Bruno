#include "brpch.h"
#include "Model.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(Model);

	/*Model::Model(std::vector<Mesh>&& meshes) :
		m_meshes(std::move(meshes))
	{
	}*/

	Mesh::Mesh(std::vector<Math::Vector3>&& vertices, std::vector<Math::Vector3>&& normals, std::vector<std::vector<Math::Vector3>>&& texCoords) :
		m_vertices(std::move(vertices)),
		m_normals(std::move(normals)),
		m_textureCoordinates(std::move(texCoords))
	{
	}
}