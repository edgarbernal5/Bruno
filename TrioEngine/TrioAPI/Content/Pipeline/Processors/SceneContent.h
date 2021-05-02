#pragma once

#include "TrioApiRequisites.h"

#include "Content/Pipeline/ContentItem.h"

#include <string>
#include <vector>

namespace TrioEngine
{
	class SceneMeshContent;
	class ModelBoneContent;
	class ModelMeshPartContent;
	class ContentWriter;
	class MaterialContent;

	class TRIO_API_EXPORT SceneContent : public ContentItem
	{
	public:
		SceneContent(ModelBoneContent* root, std::vector<ModelBoneContent*> bones, std::vector<MaterialContent*> materials, std::vector<SceneMeshContent*> meshes);
		SceneContent();
		~SceneContent();

		inline std::vector<MaterialContent*>& GetMaterials() {
			return m_materials;
		}

		friend class SceneWriter;
	private:
		std::vector<ModelBoneContent*> m_bones;
		std::vector<SceneMeshContent*> m_meshes;
		std::vector<MaterialContent*> m_materials;

		ModelBoneContent* m_root;
	};
}