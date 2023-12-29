#pragma once

#include "Bruno/Core/RTTI.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"
#include "Bruno/Math/Math.h"

#include <string>

namespace Bruno
{
	class Mesh;
	class Material;

	struct ModelVertex
	{
		Math::Vector3 Position;
		Math::Vector3 Normal;
		Math::Vector3 Tangent;
		Math::Vector3 Binormal;
		Math::Vector3 Texcoord;
	};

	class Model : public RTTI
	{
		BR_RTTI_DECLARATION(Model, RTTI);

	public:
		Model(std::vector<ModelVertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<std::shared_ptr<Material>>&& materials, std::vector<std::shared_ptr<Mesh>>&& meshes);
		//Model(std::vector<std::shared_ptr<Mesh>>&& meshes);

		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() { return m_indexBuffer; }
		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() { return m_vertexBuffer; }

		void SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer);
		void SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer);

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_meshes; }
		std::shared_ptr<Material>& GetMaterial(uint32_t materialIndex);
	
	private:
		std::vector<std::shared_ptr<Material>> m_materials;
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<ModelVertex> m_vertices;
		std::vector<uint32_t> m_indices;

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};

	struct ModelNode
	{
		uint32_t Parent = 0xFFFFFFFF;
		std::vector<uint32_t> Children;
		std::vector<uint32_t> Meshes;

		std::string Name;
		Math::Matrix LocalTransform;

		inline bool IsRoot() const { return Parent == 0xFFFFFFFF; }
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(Mesh&&) = default;
		Mesh(const std::string& name, uint32_t baseVertex, uint32_t baseIndex, uint32_t vertexCount, uint32_t indexCount, uint32_t materialIndex, const Math::Matrix& transform, const Math::Matrix& localTransform, const Math::BoundingBox& bbox);
		//Mesh(std::vector<Math::Vector3> && vertices, std::vector<Math::Vector3>&& normals, std::vector<Math::Vector3>&& tangets, std::vector<Math::Vector3>&& birnormals, std::vector < std::vector<Math::Vector3>> && textureCoordinates);

		uint32_t GetBaseVertex() const { return m_baseVertex; }
		uint32_t GetBaseIndex() const { return m_baseIndex; }
		uint32_t GetIndexCount() const { return m_indexCount; }
		uint32_t GetVertexCount() const { return m_vertexCount; }
		uint32_t GetMaterialIndex() const { return m_materialIndex; }

		Mesh& operator=(Mesh&&) = default;
	private:
		std::string m_meshName;
		std::string m_nodeName;
		Math::Matrix m_transform;
		Math::Matrix m_localTransform;
		uint32_t m_baseVertex;
		uint32_t m_baseIndex;
		uint32_t m_materialIndex;
		uint32_t m_indexCount;
		uint32_t m_vertexCount;
		Math::BoundingBox m_bBox;
	};
}
