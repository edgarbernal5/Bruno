#pragma once

#include "Math/MathVector.h"

namespace BrunoEngine
{
	enum class LightType {
		Directional,
		SpotLight,

	};

	struct LightComponent
	{
		LightType m_type;
		Vector3 m_color;
		Vector3 m_lightDirection;
	};
}