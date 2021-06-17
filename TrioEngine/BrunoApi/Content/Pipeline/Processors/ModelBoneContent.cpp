#include "stdafx.h"
#include "ModelBoneContent.h"

namespace BrunoEngine
{
	ModelBoneContent::ModelBoneContent()
	{
	}

	ModelBoneContent::ModelBoneContent(std::string name, int index, Matrix transform, ModelBoneContent* parent) :
		m_name(name),
		m_index(index),
		m_transform(transform),
		m_parent(parent)
	{
	}

	ModelBoneContent::~ModelBoneContent() {

	}
}