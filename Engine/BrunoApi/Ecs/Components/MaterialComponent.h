#pragma once

#include "Math/MathVector.h"
#include "Graphics/Texture2D.h"

namespace BrunoEngine
{
	struct MaterialComponent
	{
		Vector4 baseColor = Vector4(1.0f);

		Texture2D* diffuseTexture;
	};
}