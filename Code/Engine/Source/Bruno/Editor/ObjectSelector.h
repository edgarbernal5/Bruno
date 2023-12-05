#pragma once

#include <vector>
#include "Bruno/Math/Math.h"

namespace Bruno
{
	class ObjectSelector
	{
	public:
		ObjectSelector();
		std::vector<Math::Matrix>& GetSelectedObjects() { return m_objects; }

	private:
		std::vector<Math::Matrix> m_objects;
	};
}

