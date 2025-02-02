#pragma once

#include "Bruno/Core/RTTI.h"
#include "Bruno/Content/Asset.h"
#include "Bruno/Platform/DirectX/IndexBuffer.h"
#include "Bruno/Platform/DirectX/VertexBuffer.h"
#include "Bruno/Math/Math.h"

#include <string>
#include <map>
#include <Bruno/Content/AssetTypes.h>

namespace Bruno
{
	class Mesh;
	class Material;
	class Texture;

	struct ModelVertex
	{
		Math::Vector3 Position;
		Math::Vector3 Normal;
		Math::Vector3 Tangent;
		Math::Vector3 Binormal;
		Math::Vector3 Texcoord;
	};

	struct ModelNode
	{
		static constexpr uint32_t NullNode = 0xFFFFFFFF;
		uint32_t Parent = NullNode;
		std::vector<uint32_t> Children;
		std::vector<uint32_t> Meshes;

		std::string Name;
		Math::Matrix LocalTransform;

		inline bool IsRoot() const { return Parent == NullNode; }
	};

	enum class TextureSemantic : uint8_t
	{
		Unknown = 0,
		DiffuseMap,
		NormalMap
	};

	enum class PropertySemantic : uint8_t
	{
		Unknown = 0,
		DiffuseColor
	};

	class ModelMaterial : public RTTI
	{
	public:
		BR_RTTI_DECLARATION(ModelMaterial, RTTI);

	public:
		std::string Name;
		std::map<TextureSemantic, std::string> TexturesBySemantic;
		std::map<PropertySemantic, std::string> Properties;
		std::map<std::string, AssetHandle> TexturesByName;
	};

	class Model : public Asset
	{
		BR_RTTI_DECLARATION(Model, Asset);

	public:
		Model(std::vector<ModelVertex>&& vertices, std::vector<uint32_t>&& indices, std::vector<std::shared_ptr<Material>>&& materials, 
			std::vector<std::shared_ptr<Mesh>>&& meshes, std::vector<ModelNode>&& modelNodes);

		AssetType GetAssetType() const override { return AssetType::Model; }

		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }
		const std::shared_ptr<VertexBuffer>& GetVertexBuffer() const { return m_vertexBuffer; }

		void SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer);
		void SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer);

		const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return m_meshes; }
		std::shared_ptr<Material>& GetMaterial(uint32_t materialIndex);
		std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_materials; }

		const ModelNode& GetRootNode() const { return m_modelNodes[0]; }
		const std::vector<ModelNode>& GetNodes() const { return m_modelNodes; }
	
	private:
		std::vector<std::shared_ptr<Material>> m_materials;
		std::vector<std::shared_ptr<Mesh>> m_meshes;
		std::vector<ModelVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<ModelNode> m_modelNodes;

		std::shared_ptr<VertexBuffer> m_vertexBuffer;
		std::shared_ptr<IndexBuffer> m_indexBuffer;
	};

	class Mesh
	{
	public:
		Mesh() = default;
		Mesh(Mesh&&) = default;
		Mesh(const std::string& name, uint32_t baseVertex, uint32_t baseIndex, uint32_t vertexCount, uint32_t indexCount, uint32_t materialIndex, 
			const Math::Matrix& transform, const Math::Matrix& localTransform, const Math::BoundingBox& bbox);
		
		uint32_t GetBaseVertex() const { return m_baseVertex; }
		uint32_t GetBaseIndex() const { return m_baseIndex; }
		uint32_t GetIndexCount() const { return m_indexCount; }
		uint32_t GetVertexCount() const { return m_vertexCount; }
		uint32_t GetMaterialIndex() const { return m_materialIndex; }
		const Math::BoundingBox& GetBoundingBox() const { return m_bBox; }
		const Math::Matrix& GetTransform() const { return m_transform; }

		void SetNodeName(const std::string& nodename) { m_nodeName = nodename; }
		void SetTransform(const Math::Matrix& transform) { m_transform = transform; }
		void SetLocalTransform(const Math::Matrix& transform) { m_localTransform = transform; }
		void SetBoundingBox(const Math::BoundingBox& box) { m_bBox = box; }
		
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
