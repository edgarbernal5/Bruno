#pragma once

#include "Bruno/Math/Math.h"
#include "Bruno/Core/UUID.h"
#include "Bruno/Renderer/MaterialMap.h"
#include <vector>
#include <string>

#include "Bruno/Platform/DirectX/ConstantBuffer.h"

namespace Bruno
{
	struct IdComponent
	{
		UUID Id{ 0 };
	};

	struct NameComponent
	{
		std::wstring Name;

		NameComponent() = default;
		NameComponent(const NameComponent& other) = default;
		NameComponent(std::wstring name) : Name(name) { }
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

		void ApplyTransform(const Math::Matrix& localTransform)
		{
			Math::Matrix matrix = localTransform;
			bool isValid = matrix.Decompose(Scale, Rotation, Position);
			
			if (!isValid)
			{
				BR_CORE_TRACE << "Matrix decompose not valid!" << std::endl;
			}
		}

		Math::Matrix GetTransform() const
		{
			return Math::Matrix::CreateScale(Scale) *
				Math::Matrix::CreateFromQuaternion(Rotation) *
				Math::Matrix::CreateTranslation(Position);
		}
	};
	
	struct CBVComponent 
	{
		std::shared_ptr<ConstantBuffer> TransformCB[2]; 
	};
	
	struct BoundingBoxComponent
	{
		Math::Vector3 Center;  // En espacio local
		Math::Vector3 Extents; // Mitad del tamaño (Half-size)
	};
	
	struct ModelComponent
	{
		AssetHandle ModelHandle;
		uint32_t MeshIndex{ 0 };
		std::shared_ptr<MaterialMap> Materials = std::make_shared< MaterialMap>();
	};
}
