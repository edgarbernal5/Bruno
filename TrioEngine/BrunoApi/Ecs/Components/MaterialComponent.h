#pragma once

#include "Math/MathVector.h"
#include "Graphics/Texture2D.h"

namespace BrunoEngine
{
	//class Texture2D;

	struct MaterialComponent
	{
		Vector4 baseColor = Vector4(1);

		Texture2D* diffuseTexture;
	};
}