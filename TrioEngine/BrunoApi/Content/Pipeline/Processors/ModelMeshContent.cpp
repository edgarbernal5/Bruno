#include "stdafx.h"
#include "ModelMeshContent.h"

namespace BrunoEngine
{
	ModelMeshContent::ModelMeshContent()
	{
	}

	ModelMeshContent::ModelMeshContent(std::string name, MeshContent* sourceMesh, ModelBoneContent* parentBone, BoundingSphere boundingSphere, BoundingBox boundingBox, std::vector<ModelMeshPartContent*>& meshParts) :
		m_name(name),
		m_parentBone(parentBone),
		m_boundingSphere(boundingSphere),
		m_boundingBox(boundingBox),
		m_sourceMesh(sourceMesh),
		m_meshParts(meshParts)
	{
	}

	ModelMeshContent::~ModelMeshContent()
	{
	}
}