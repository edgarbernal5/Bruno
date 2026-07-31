#pragma once

#include "Bruno/Math/Math.h"
#include "Bruno/Core/UUID.h"
#include "Bruno/Renderer/MaterialMap.h"
#include <vector>
#include <string>
#include <entt/entt.hpp>

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
		entt::entity Parent{ entt::null };
		entt::entity FirstChild{ entt::null };
		entt::entity NextSibling{ entt::null };
		entt::entity PrevSibling{ entt::null };
		
		// (Opcional) Puedes conservar los UUIDs aquí SOLO para cuando guardes 
		// la escena en un JSON/YAML, pero no los uses en el Game Loop.
		
		HierarchyComponent() = default;
		HierarchyComponent(const HierarchyComponent& other) = default;
	};

	struct TransformComponent
	{
		Math::Vector3 Position;
		Math::Vector3 Scale{ 1.0f, 1.0f, 1.0f };
		Math::Quaternion Rotation;
		
		Math::Matrix LocalTransform = Math::Matrix::Identity;
		Math::Matrix WorldTransform = Math::Matrix::Identity;

		bool IsDirty = true;
		
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
		
		Math::Vector3 GetWorldPosition() const
		{
			return WorldTransform.Translation();
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
