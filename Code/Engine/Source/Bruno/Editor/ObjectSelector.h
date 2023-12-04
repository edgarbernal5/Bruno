#pragma once

#include <vector>

namespace Bruno
{
	class ObjectSelector
	{
	public:
		std::vector<int>& GetSelectedObjects() { return m_objects; }
	private:
		std::vector<int> m_objects;
	};
}

