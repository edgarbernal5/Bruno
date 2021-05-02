#include "stdafx.h"
#include "ModelBone.h"

namespace TrioEngine
{
	ModelBone::ModelBone() : m_parent(nullptr), m_index(-1)
	{

	}

	ModelBone::ModelBone(std::string name, Matrix transform, int index) : m_parent(nullptr), m_index(index)
	{
		m_transform = transform;
	}


	ModelBone::~ModelBone()
	{
	}
}