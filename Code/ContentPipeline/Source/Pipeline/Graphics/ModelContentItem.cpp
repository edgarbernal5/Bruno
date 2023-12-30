#include "ModelContentItem.h"

#include "MaterialContentItem.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelContentItem);

	ModelContentItem::ModelContentItem(std::vector<ModelVertexContentItem>&& vertices, std::vector<uint32_t>&& indices, 
		std::vector<std::shared_ptr<MaterialContentItem>>&& materials, std::vector<std::shared_ptr<MeshContentItem>>&& meshes,
		std::vector<std::shared_ptr<ModelNodeContentItem>>&& modelNodes) :
		Vertices(std::move(vertices)),
		Indices(std::move(indices)),
		Materials(std::move(materials)),
		Meshes(std::move(meshes)),
		ModelNodes(std::move(modelNodes))
	{
	}

	BR_RTTI_DEFINITIONS(ModelNodeContentItem);

	BR_RTTI_DEFINITIONS(MeshContentItem);
}
