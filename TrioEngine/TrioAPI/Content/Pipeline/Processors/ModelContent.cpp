#include "stdafx.h"
#include "ModelContent.h"


namespace TrioEngine
{
	ModelContent::ModelContent() :
		m_root(nullptr)
	{
	}

	ModelContent::ModelContent(ModelBoneContent* root, std::vector<ModelBoneContent*> bones, std::vector<ModelMeshContent*> meshes) :
		m_root(root),
		m_bones(bones),
		m_meshes(meshes)
	{

	}

	ModelContent::~ModelContent()
	{
	}

	
}