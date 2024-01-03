#pragma once

#include "Bruno/Math/Math.h"
#include <vector>

namespace Bruno
{
	using UUID = uint64_t;

	struct IdComponent
	{
		UUID Id{ 0 };
	};

	struct HierarchyComponent
	{
		UUID Parent{ 0 };
		std::vector<UUID> Children;

		HierarchyComponent() = default;
		HierarchyComponent(const HierarchyComponent& other) = default;
		HierarchyComponent(UUID parent) : Parent(parent) {}
	};

	struct TransformComponent
	{
		Math::Vector3 Position;
		Math::Vector3 Scale{ 1.0f, 1.0f, 1.0f };
		Math::Quaternion Rotation;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
	};

	struct ModelComponent
	{
		UUID ModelHandle;
		uint32_t SubmeshIndex{ 0 };
	};
}