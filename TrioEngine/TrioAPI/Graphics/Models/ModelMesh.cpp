#include "stdafx.h"
#include "ModelMesh.h"


namespace TrioEngine
{
	ModelMesh::ModelMesh() : m_parentBone(nullptr)
	{

	}

	ModelMesh::ModelMesh(std::string name, ModelBone* parentBone/*, BoundingSphere boundingSphere, BoundingBox boundingBox*/, std::vector<ModelMeshPart*>& meshParts) :
		m_name(name), m_parentBone(parentBone), m_ModelMeshParts(meshParts)/*, m_boundingSphere(boundingSphere), m_boundingBox(boundingBox)*/
	{
	}

	ModelMesh::~ModelMesh()
	{
	}
}