#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class ModelMeshContent;
	class ModelBoneContent;
	class ModelMeshPartContent;
	class ContentWriter;

	class TRIO_API_EXPORT ModelContent : public ContentItem
	{
	public:
		ModelContent(ModelBoneContent* root, std::vector<ModelBoneContent*> bones, std::vector<ModelMeshContent*> meshes);
		ModelContent();
		~ModelContent();

		friend class ModelWriter;
	private:
		std::vector<ModelBoneContent*> m_bones;
		std::vector<ModelMeshContent*> m_meshes;

		ModelBoneContent* m_root;
	};
}