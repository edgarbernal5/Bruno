#include "SceneDocumentPanel.h"

#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <Bruno/Scene/Scene.h>

#include "Panels/ScenePanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Scene/SelectionService.h"
#include "Gizmos/GizmoService.h"
#include "EditorGame.h"

namespace Bruno
{
	SceneDocumentPanel::SceneDocumentPanel(nana::window window, EditorGame* editorGame, std::shared_ptr<Scene> scene) :
		nana::panel<true>(window),
		m_editorGame(editorGame),
		m_scene(scene)
	{
		this->caption("Scene");

		InitializeCamera();
		InitializeGizmoService();

		m_place.bind(this->handle());
		////////// VIEW
		m_place.div("<dock>");

		nana::pane_info paneInfo;
		paneInfo.show_close_button = false;
		paneInfo.id = "pane1";
		auto sceneHierarchyPanel = m_place.add_pane<SceneHierarchyPanel>(paneInfo, "", nana::dock_position::right, scene, m_selectionService, m_gizmoService);

		paneInfo.show_caption = false;
		paneInfo.id = "pane3";
		auto scenePanel = m_place.add_pane<ScenePanel>(paneInfo, "pane1", nana::dock_position::right, editorGame, &m_camera, scene, m_selectionService, m_gizmoService);

		paneInfo.id = "pane2";
		paneInfo.show_caption = true;
		paneInfo.caption = "Properties";
		auto propertiesPanel = m_place.add_pane<PropertiesPanel>(paneInfo, "pane1", nana::dock_position::down, scene, m_selectionService);

		this->events().expose([scenePanel](const nana::arg_expose& arg)
		{
			if (arg.exposed)
				scenePanel->show();
			else
				scenePanel->hide();
		});

		m_place.collocate();
	}

	SceneDocumentPanel::~SceneDocumentPanel()
	{
	}

	void SceneDocumentPanel::InitializeCamera()
	{
		m_camera.LookAt(Math::Vector3(0, 0, -25), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0, 0, 1, 1), 1.0f, 1000.0f);
	}

	void SceneDocumentPanel::InitializeGizmoService()
	{
		auto device = Graphics::GetDevice();
		m_selectionService = std::make_shared<SelectionService>(m_scene, m_editorGame->GetAssetManager());

		m_gizmoService = std::make_shared<GizmoService>(device, m_camera, m_selectionService.get());
		m_gizmoService->SetTranslationCallback([&](const Math::Vector3& delta)
		{
			auto& selections = m_selectionService->GetSelections();
			for (auto& uuid : selections)
			{
				Entity entity = m_scene->TryGetEntityWithUUID(uuid);
				TransformComponent& entityTransform = entity.GetComponent<TransformComponent>();

				entityTransform.Position += delta;
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
}