#pragma once

#include <vector>
#include "Pipeline/ContentItem.h"
#include "Bruno/Math/Math.h"

namespace Bruno
{
	struct MaterialContentItem;
	struct MeshContentItem;
	struct ModelNodeContentItem;

	struct ModelVertexContentItem
	{
		Math::Vector3 Position;
		Math::Vector3 Normal;
		Math::Vector3 Tangent;
		Math::Vector3 Binormal;
		Math::Vector3 Texcoord;
	};

	struct ModelContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(ModelContentItem, ContentItem);

	public:
		ModelContentItem(std::vector<ModelVertexContentItem>&& vertices, std::vector<uint32_t>&& indices, std::vector<std::shared_ptr<MaterialContentItem>>&& materials, std::vector<std::shared_ptr<MeshContentItem>>&& meshes, std::vector<std::shared_ptr<ModelNodeContentItem>>&& modelNodes);

		std::vector<ModelVertexContentItem> Vertices;
		std::vector<uint32_t> Indices;

		std::vector<std::shared_ptr<MaterialContentItem>> Materials;
		std::vector<std::shared_ptr<MeshContentItem>> Meshes;
		std::vector<std::shared_ptr<ModelNodeContentItem>> ModelNodes;
	};

	struct ModelNodeContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(ModelNodeContentItem, ContentItem);

	public:
		uint32_t Parent{ 0xFFFFFFFF };
		std::vector<uint32_t> Children;
		std::vector<uint32_t> Meshes;

		std::string Name;
		Math::Matrix LocalTransform;
	};

	struct MeshContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(MeshContentItem, ContentItem);

	public:
		uint32_t BaseVertex{ 0 };
		uint32_t BaseIndex{ 0 };
		uint32_t VertexCount{ 0 };
		uint32_t IndexCount{ 0 };
		uint32_t MaterialIndex{ 0 };
		std::string MeshName;
		std::string NodeName;
		Math::Matrix Transform;
		Math::Matrix LocalTransform;
		//std::vector<Math::Vector3> Vertices;
		//std::vector<Math::Vector3> Normals;
		//std::vector<Math::Vector3> Tangents;
		//std::vector<Math::Vector3> BiNormals;
		//std::vector<std::vector<Math::Vector3>> TextureCoordinates;
		//std::vector<std::vector<Math::Vector4>> VertexColors;
		//std::uint32_t FaceCount{ 0 };
		//std::vector<std::uint32_t> Indices;
		Math::BoundingBox BBox;
	};

}