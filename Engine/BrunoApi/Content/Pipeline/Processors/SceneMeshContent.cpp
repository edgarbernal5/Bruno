#include "stdafx.h"
#include "SceneMeshContent.h"

namespace BrunoEngine
{
	SceneMeshContent::SceneMeshContent()
	{
	}

	SceneMeshContent::SceneMeshContent(std::string name, MeshContent* sourceMesh, ModelBoneContent* parentBone, /*BoundingSphere boundingSphere, BoundingBox boundingBox,*/ std::vector<SceneSubMeshContent*>& meshParts) :
		m_name(name),
		m_parentBone(parentBone),
		/*m_boundingSphere(boundingSphere),
		m_boundingBox(boundingBox),*/
		m_sourceMesh(sourceMesh),
		m_meshParts(meshParts)
	{
	}

	SceneMeshContent::~SceneMeshContent()
	{
	}
}