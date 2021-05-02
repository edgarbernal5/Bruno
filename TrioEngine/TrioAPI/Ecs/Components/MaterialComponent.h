#pragma once

#include "Math/MathVector.h"

namespace TrioEngine
{
	class Texture2D;

	struct MaterialComponent
	{
		Vector4 baseColor = Vector4(1);

		Texture2D* diffuseTexture;
	};
}