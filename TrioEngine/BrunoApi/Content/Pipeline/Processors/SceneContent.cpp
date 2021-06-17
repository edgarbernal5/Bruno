#include "stdafx.h"
#include "SceneContent.h"

namespace TrioEngine
{
	SceneContent::SceneContent() :
		m_root(nullptr)
	{
	}

	SceneContent::SceneContent(ModelBoneContent* root, std::vector<ModelBoneContent*> bones, std::vector<MaterialContent*> materials, std::vector<SceneMeshContent*> meshes) :
		m_root(root),
		m_bones(bones),
		m_materials(materials),
		m_meshes(meshes)
	{

	}

	SceneContent::~SceneContent()
	{
	}

	
}