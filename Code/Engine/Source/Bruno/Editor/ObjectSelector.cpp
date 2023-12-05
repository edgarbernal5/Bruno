#include "brpch.h"
#include "ObjectSelector.h"

namespace Bruno
{
	ObjectSelector::ObjectSelector()
	{
		m_objects.push_back(Math::Matrix::Identity);
	}
}