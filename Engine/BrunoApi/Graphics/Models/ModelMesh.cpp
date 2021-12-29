#include "stdafx.h"
#include "ModelMesh.h"

namespace BrunoEngine
{
	ModelMesh::ModelMesh() : m_parentBone(nullptr)
	{

	}

	ModelMesh::ModelMesh(std::string name, ModelBone* parentBone, Matrix& transform, BoundingSphere boundingSphere, BoundingBox boundingBox, std::vector<ModelMeshPart*>& meshParts) :
		m_name(name), m_parentBone(parentBone), m_transform(transform), m_modelMeshParts(meshParts), m_boundingSphere(boundingSphere), m_boundingBox(boundingBox)
	{

	}

	ModelMesh::~ModelMesh()
	{
	}
}