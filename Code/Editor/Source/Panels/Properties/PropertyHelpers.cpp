#include "PropertyHelpers.h"

namespace Bruno
{
	Math::Vector3 Property::AsVector3(const std::string& value, const Math::Vector3& default_value)
	{
        std::istringstream builder(value);
        Math::Vector3 result;
        char separator;
        builder >> result.x >> separator >> result.y >> separator >> result.z;

        return result;
	}
}
