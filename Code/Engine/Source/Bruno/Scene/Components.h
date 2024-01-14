#pragma once

#include "Bruno/Math/Math.h"
#include "Bruno/Core/UUID.h"
#include "Bruno/Renderer/MaterialMap.h"
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

		void ApplyTransform(const Math::Matrix& transform)
		{
			Math::Matrix matrix = transform;
			matrix.Decompose(Scale, Rotation, Position);
		}

		Math::Matrix GetTransform() const
		{
			return Math::Matrix::CreateScale(Scale) *
				Math::Matrix::CreateFromQuaternion(Rotation) *
				Math::Matrix::CreateTranslation(Position);
		}
	};

	struct ModelComponent
	{
		AssetHandle ModelHandle;
		uint32_t MeshIndex{ 0 };
		MaterialMap Materials;
	};
}