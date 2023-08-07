#include "ModelContentItem.h"

#include "MaterialContentItem.h"

namespace Bruno
{
	BR_RTTI_DEFINITIONS(ModelContentItem);

	ModelContentItem::ModelContentItem(std::vector<std::shared_ptr<MaterialContentItem>>&& materials, std::vector<std::shared_ptr<MeshContentItem>>&& meshes) :
		Materials(std::move(materials)),
		Meshes(std::move(meshes))
	{
	}

	BR_RTTI_DEFINITIONS(MeshContentItem);
}
