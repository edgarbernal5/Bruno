#pragma once

#include "Bruno/Core/RTTI.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"

#include <string>

namespace Bruno
{
	class Mesh;

	class Model : public RTTI
	{
		BR_RTTI_DECLARATION(Model, RTTI);

	public:
		Model(std::vector<std::shared_ptr<Mesh>>&& meshes);

	private:
		std::vector<std::shared_ptr<Mesh>> m_meshes;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(Mesh&&) = default;
		Mesh(std::vector<Math::Vector3> && vertices, std::vector<Math::Vector3>&& normals, std::vector<std::vector<Math::Vector3>>&& texCoords);

		//Mesh(std::vector<Math::Vector3> && vertices, std::vector<Math::Vector3>&& normals, std::vector<Math::Vector3>&& tangets, std::vector<Math::Vector3>&& birnormals, std::vector < std::vector<Math::Vector3>> && textureCoordinates);

		Mesh& operator=(Mesh&&) = default;
	private:
		std::vector<Math::Vector3> m_vertices;
		std::vector<Math::Vector3> m_normals;
		std::vector<Math::Vector3> m_tangents;
		std::vector<Math::Vector3> m_binormals;

		std::vector < std::vector<Math::Vector3>> m_textureCoordinates;

		std::shared_ptr<VertexBuffer> VertexB;
		std::shared_ptr<IndexBuffer> IndexB;
	};
}
