#pragma once

#include <Bruno/Math/Math.h>
#include <string>
#include <sstream>

namespace Bruno
{
	class Property
	{
	public:
		static Math::Vector3 AsVector3(const std::string& value, const Math::Vector3& default_value = Math::Vector3());
	};
}