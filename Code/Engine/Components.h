#pragma once

#include "Bruno/Math/Math.h"

namespace Bruno
{
	struct TransformComponent
	{
		Math::Vector3 Position;
		Math::Vector3 Scale{ 1.0f, 1.0f,1.0f };
		Math::Quaternion Rotation;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
	};

	struct ModelComponent
	{

	};
}