#pragma once

#include "Bruno/Math/Math.h"
#include "Bruno/Core/UUID.h"
#include <vector>
#include <string>

namespace Bruno
{
	struct IdComponent
	{
		UUID Id{ 0 };
	};

	struct NameComponent
	{
		std::string Name;
		NameComponent() = default;
		NameComponent(const NameComponent& other) = default;
		NameComponent(std::string name) : Name(name) { }
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