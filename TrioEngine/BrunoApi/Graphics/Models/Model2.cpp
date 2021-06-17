#include "stdafx.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "ModelMaterial.h"
#include "Mesh.h"

#include "Errors/GameException.h"

namespace TrioEngine
{
	Model::Model(const std::string& filename, ModelLoadParams loadParams)
	{
		Assimp::Importer importer;

		uint32_t flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		if (loadParams.flipUVs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(filename, flags);
		if (scene == nullptr)
		{
			throw GameException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				m_materials.push_back(new ModelMaterial(scene->mMaterials[i]));
			}
		}

		if (scene->HasMeshes())
		{
			for (uint32_t i = 0; i < scene->mNumMeshes; i++)
			{
				Mesh* mesh = new Mesh(*this, *(scene->mMeshes[i]));
				m_meshes.push_back(mesh);
			}
		}

		if (scene->HasAnimations())
		{
			/*assert(scene->mRootNode != nullptr);
			m_pRootNode = BuildSkeleton(*scene->mRootNode, nullptr);

			mAnimations.reserve(scene->mNumAnimations);
			for (UINT i = 0; i < scene->mNumAnimations; i++)
			{
				AnimationClip* animationClip = new AnimationClip(*this, *(scene->mAnimations[i]));
				mAnimations.push_back(animationClip);
				mAnimationsByName.insert(std::pair<std::string, AnimationClip*>(animationClip->Name(), animationClip));
			}*/
		}
	}

	Model::~Model()
	{
		for (Mesh* mesh : m_meshes)
		{
			delete mesh;
		}

		for (ModelMaterial* material : m_materials)
		{
			delete material;
		}

		m_meshes.clear();
		m_materials.clear();
	}

	const std::vector<ModelMaterial*>& Model::GetMaterials() const
	{
		return m_materials;
	}

	const std::vector<Mesh*>& Model::GetMeshes() const
	{
		return m_meshes;
	}
}