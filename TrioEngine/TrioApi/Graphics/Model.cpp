#include "stdafx.h"
#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace TrioEngine
{
	Model::Model(const std::string& filename, ModelLoadParams loadParams)
	{
		Assimp::Importer importer;

		UINT flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_FlipWindingOrder;
		if (loadParams.flipUVs)
		{
			flags |= aiProcess_FlipUVs;
		}

		const aiScene* scene = importer.ReadFile(filename, flags);
		if (scene == nullptr)
		{
			//throw GameException(importer.GetErrorString());
		}

		if (scene->HasMaterials())
		{
			for (UINT i = 0; i < scene->mNumMaterials; i++)
			{
				//mMaterials.push_back(new ModelMaterial(*this, scene->mMaterials[i]));
			}
		}

		if (scene->HasMeshes())
		{
			for (UINT i = 0; i < scene->mNumMeshes; i++)
			{
				//Mesh* mesh = new Mesh(*this, *(scene->mMeshes[i]));
				//mMeshes.push_back(mesh);
			}
		}
	}

	Model::~Model()
	{

	}


	const std::vector<Mesh*>& Model::GetMeshes() const
	{
		return m_vMeshes;
	}
}