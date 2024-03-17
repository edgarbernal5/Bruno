#include "SceneDocument.h"

#include <Bruno/Scene/Scene.h>

#include "SelectionService.h"
#include "SceneHierarchy.h"
#include "Gizmos/GizmoService.h"
#include "Panels/Properties/PropertyHelpers.h"

namespace Bruno
{
	SceneDocument::SceneDocument(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
		InitializeCamera();
		InitializeGizmoService();

		m_sceneHierarchy = std::make_shared<SceneHierarchy>(scene);
		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			auto entityUUID = selection.size() > 0 ? selection[0] : UUID(0);
			if (entityUUID) {
				auto worldMatrix = m_scene->GetWorldSpaceMatrix(m_scene->GetEntityWithUUID(entityUUID));
				m_gizmoService->SetGizmoPosition(worldMatrix.Translation());
			}
			m_gizmoService->SetActive(entityUUID);

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

		m_sceneHierarchy->Load(rootEntity);

		InitializeProperties(rootEntity);

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
		m_selectionService = std::make_shared<SelectionService>(m_scene, m_assetManager);

		m_gizmoService = std::make_shared<GizmoService>(device, m_camera, m_selectionService.get());
		m_gizmoService->SetTranslationCallback([&](const Math::Vector3& delta)
		{
			auto& selections = m_selectionService->GetSelections();
			for (auto& uuid : selections)
			{
				auto& properties = (*m_sceneHierarchy)[uuid];
				auto prop = properties.get("Transform/Position");
				auto currentPosition = prop.as_vector3();
				currentPosition += delta;
				prop.value(currentPosition);
			}
		});

		m_gizmoService->SetRotationCallback([&](const Math::Quaternion& delta)
		{
			auto& selections = m_selectionService->GetSelections();
			for (auto& uuid : selections)
			{
				Entity entity = m_scene->TryGetEntityWithUUID(uuid);
				TransformComponent& entityTransform = entity.GetComponent<TransformComponent>();
				auto newRotation = entityTransform.Rotation * delta;
				newRotation.Normalize();

				entityTransform.Rotation = newRotation;
			}
		});

		m_gizmoService->SetScaleCallback([&](const Math::Vector3& delta, bool isUniform)
		{
			auto newDelta = delta * 0.1f;
			auto& selections = m_selectionService->GetSelections();
			for (auto& uuid : selections)
			{
				Entity entity = m_scene->TryGetEntityWithUUID(uuid);
				TransformComponent& entityTransform = entity.GetComponent<TransformComponent>();

				if (isUniform)
				{
					float uniformDelta = 1.0f + (newDelta.x + newDelta.y + newDelta.z) / 3.0f;
					auto newScale = entityTransform.Scale * uniformDelta;
					if (newScale.x > 0.001f && newScale.y > 0.001f && newScale.z > 0.001f)
					{
						entityTransform.Scale = newScale;
					}

					continue;
				}
				auto newScale = entityTransform.Scale + newDelta;
				if (newScale.x > 0.001f && newScale.y > 0.001f && newScale.z > 0.001f)
				{
					entityTransform.Scale = newScale;
				}
			}
		});
	}

	void SceneDocument::InitializeProperties(Entity entity)
	{
		auto& hierarchy = entity.GetComponent<HierarchyComponent>();
		auto& name = entity.GetComponent<NameComponent>().Name;

		auto uuid = entity.GetUUID();

		auto properties = (*m_sceneHierarchy)[uuid];
		properties.get("Transform/Position").on_change().connect([this, uuid](const std::string& new_value)
		{
			Entity entity = m_scene->TryGetEntityWithUUID(uuid);
			TransformComponent& entityTransform = entity.GetComponent<TransformComponent>();

			entityTransform.Position = Property::AsVector3(new_value);
		});

		for (UUID child : hierarchy.Children)
		{
			auto childEntity = m_scene->TryGetEntityWithUUID(child);
			if (childEntity)
			{
				InitializeProperties(childEntity);
			}
		}
	}
}