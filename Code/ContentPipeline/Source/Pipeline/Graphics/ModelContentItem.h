#pragma once

#include <vector>
#include "Pipeline/ContentItem.h"
#include "Bruno/Math/Math.h"

namespace Bruno
{
	struct MaterialContentItem;
	struct MeshContentItem;

	struct ModelContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(ModelContentItem, ContentItem);

	public:
		ModelContentItem(std::vector<std::shared_ptr<MaterialContentItem>>&& materials, std::vector<std::shared_ptr<MeshContentItem>>&& meshes);

		std::vector<std::shared_ptr<MaterialContentItem>> Materials;
		std::vector<std::shared_ptr<MeshContentItem>> Meshes;
	};

	struct MeshContentItem : public ContentItem
	{
		BR_RTTI_DECLARATION(MeshContentItem, ContentItem);

	public:
		std::string Name;
		std::vector<Math::Vector3> Vertices;
		std::vector<Math::Vector3> Normals;
		std::vector<Math::Vector3> Tangents;
		std::vector<Math::Vector3> BiNormals;
		std::vector<std::vector<Math::Vector3>> TextureCoordinates;
		std::vector<std::vector<Math::Vector4>> VertexColors;
		std::uint32_t FaceCount{ 0 };
		std::vector<std::uint32_t> Indices;
		uint32_t MaterialIndex{ 0 };
	};

}