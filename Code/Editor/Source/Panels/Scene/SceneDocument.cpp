#include "brepch.h"
#include "SceneDocument.h"

#include <Bruno/Scene/Scene.h>

#include "SelectionService.h"
#include "SceneHierarchy.h"
#include "Bruno/Renderer/SceneRenderer.h"
#include "Content/EditorAssetManager.h"
#include "Gizmos/GizmoService.h"
#include "Gizmos/GizmoService_Gem.h"
#include "Bruno/Platform/DirectX/Shader_Gem.h"
#include "Panels/Properties/PropertyHelpers.h"

namespace Bruno
{
	SceneDocument::SceneDocument(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
		InitializeCamera();
		InitializeGizmoService();
		InitializeSceneRenderer();
		
		m_sceneHierarchy = std::make_shared<SceneHierarchy>(scene);
		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			auto entityUUID = selection.size() > 0 ? selection[0] : UUID(0);
			if (entityUUID)
			{
				auto worldMatrix = m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(entityUUID));
				m_dxGizmoService->SetGizmoPosition(worldMatrix.Translation());
			}
			m_gizmoService->SetActive(entityUUID);
			m_dxGizmoService->SetActive(entityUUID);
			SelectionChanged.emit(selection);
		});
	}

	SceneDocument::~SceneDocument()
	{
		m_selectionService->SelectionChanged.disconnect(m_selectionChangedHandleId);
	}

	void SceneDocument::InstantiateModel(std::shared_ptr<Model> model)
	{
		Entity rootEntity = m_scene->InstantiateModel(model);
		m_sceneRenderer->InitEntitiesForRender();
		
		HierarchyChanged.emit(rootEntity, ActionMode::Add);
	}

	void SceneDocument::UpdateSelection()
	{
		auto& selection = m_selectionService->GetSelections();
		if (selection.size() > 0)
		{
			auto entityUUID = selection[0];
			auto worldMatrix = m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(entityUUID));
			m_gizmoService->SetGizmoPosition(worldMatrix.Translation());
		}
		m_gizmoService->SetActive(selection.size() > 0);
		SelectionChanged.emit(selection);
	}

	void SceneDocument::InitializeCamera()
	{
		m_camera.LookAt(Math::Vector3(0, 0, -25), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0, 0, 1, 1), 1.0f, 1000.0f);
	}

	void SceneDocument::InitializeGizmoService()
	{
		auto device = Graphics::GetDevice();
		auto dxDevice = Graphics::GetDXDevice();
		m_selectionService = std::make_shared<SelectionService>(m_scene, m_assetManager);
		
		m_dxGizmoService = std::make_shared<DX::GizmoService>(dxDevice, m_camera);
		m_dxGizmoService->Initialize();
		m_dxGizmoService->SetTranslationCallback([&](const Math::Vector3& newPosition)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				// Usamos 'patch' para que EnTT dispare el evento 'on_update<TransformComponent>'
				entity.Patch<TransformComponent>([&newPosition](auto& transform) 
				{
					transform.Position = newPosition;
				});
			}
		});
		m_dxGizmoService->SetRotationCallback([&](const Math::Quaternion& delta)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				entity.Patch<TransformComponent>([&delta](auto& transform) 
				{
					// Asumiendo que transform.Rotation guarda los Euler Angles como Vector3
					auto currentRotation = transform.Rotation; //Math::Quaternion::CreateFromYawPitchRoll(transform.Rotation);
					currentRotation *= delta;
					transform.Rotation = currentRotation;
				});
			}
		});
		
		m_gizmoService = std::make_shared<GizmoService>(device, m_camera, m_selectionService.get());
		m_gizmoService->SetTranslationCallback([&](const Math::Vector3& delta)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				// Usamos 'patch' para que EnTT dispare el evento 'on_update<TransformComponent>'
				entity.Patch<TransformComponent>([&delta](auto& transform) 
				{
					transform.Position += delta;
				});
			}
		});

		m_gizmoService->SetRotationCallback([&](const Math::Quaternion& delta)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				entity.Patch<TransformComponent>([&delta](auto& transform) 
				{
					// Asumiendo que transform.Rotation guarda los Euler Angles como Vector3
					//auto currentRotation = Math::Quaternion::CreateFromYawPitchRoll(transform.Rotation);
					//currentRotation *= delta;
					//transform.Rotation = currentRotation.ToEuler();
				});
			}
		});

		m_gizmoService->SetScaleCallback([&](const Math::Vector3& delta, bool isUniform)
		{
			const Math::Vector3 newDelta = delta * 0.1f;

			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				entity.Patch<TransformComponent>([newDelta, isUniform](auto& transform) 
				{
					Math::Vector3 newScale = transform.Scale;

					if (isUniform)
					{
						float uniformDelta = 1.0f + (newDelta.x + newDelta.y + newDelta.z) / 3.0f;
						newScale *= uniformDelta;
					}
					else
					{
						newScale += newDelta;
					}

					// Validación simple de límites (evita escalas invertidas o colapso a 0)
					if (newScale.x > 0.001f && newScale.y > 0.001f && newScale.z > 0.001f)
					{
						transform.Scale = newScale;
					}
				});
			}
		});
	}

	void SceneDocument::InitializeSceneRenderer()
	{
		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_assetManager);
	}
}
