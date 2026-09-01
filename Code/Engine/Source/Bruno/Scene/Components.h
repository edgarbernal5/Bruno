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
	
	struct BoundingBoxComponent
	{
		Math::Vector3 Center;  // En espacio local
		Math::Vector3 Extents; // Mitad del tamaño (Half-size)
	};
	
	struct ModelComponent
	{
		AssetHandle ModelHandle;
		uint32_t MeshIndex{ 0 };
		std::shared_ptr<MaterialMap> Materials = std::make_shared<MaterialMap>();
		
		// --- CACHÉ DE RENDERIZADO (RUNTIME ONLY) ---
		// Este es el ID Bindless final. Se actualiza una sola vez al cargar la escena.
		// Cuesta 0 ciclos de CPU leerlo durante el Render Loop.
		uint32_t RuntimeMaterialIndex = 0xFFFFFFFF;
	};
	
	constexpr uint32_t NUM_CASCADES = 4;
	
	struct DirectionalLightComponent
	{
		Math::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Intensity{ 1.0f };
		Math::Vector3 Direction{ 0.0f, -1.0f, 0.0f };
        
		bool CastShadows{ true };

		Math::Matrix LightViewProj[NUM_CASCADES];
		float CascadeSplits[NUM_CASCADES];
	};
	
	struct PointLightComponent
	{
		Math::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Intensity{ 1.0f };
		float Radius{ 10.0f };
		float Falloff{ 1.0f };
	};

	struct SpotLightComponent
	{
		Math::Vector3 Color{ 1.0f, 1.0f, 1.0f };
		float Intensity{ 1.0f };
		float Range{ 15.0f };
		float InnerCutoffAngle{ 15.0f };
		float OuterCutoffAngle{ 45.0f };
	};
}
