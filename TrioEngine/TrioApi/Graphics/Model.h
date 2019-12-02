#pragma once

#include "TrioApiDecl.h"

#include <string>

namespace TrioEngine
{
	class Mesh;
	class ModelMaterial;
	
	class TRIOAPI_DECL Model
	{
	public:
		struct ModelLoadParams
		{
			bool flipUVs;
			ModelLoadParams() :flipUVs(false) {}
		};

		Model(const std::string& filename, ModelLoadParams loadParams = ModelLoadParams());
		~Model();

		const std::vector<ModelMaterial*>& GetMaterials() const;
		const std::vector<Mesh*>& GetMeshes() const;
	private:

		std::vector<ModelMaterial*> m_materials;
		std::vector<Mesh*> m_meshes;

		/*std::vector<AnimationClip*> mAnimations;
		std::map<std::string, AnimationClip*> mAnimationsByName;
		std::vector<Bone*> mBones;
		std::map<std::string, UINT> mBoneIndexMapping;
		SceneNode* mRootNode;*/
	};
}