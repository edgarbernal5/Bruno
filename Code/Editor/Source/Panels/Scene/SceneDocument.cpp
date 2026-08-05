#include "brepch.h"
#include "SceneDocument.h"

#include <Bruno/Scene/Scene.h>

#include "EditorCameraController.h"
#include "SelectionService.h"
#include "Bruno/Renderer/SceneRenderer.h"
#include "Content/EditorAssetManager.h"
#include "Gizmos/GizmoService.h"
#include "Bruno/Platform/DirectX/Shader.h"

namespace Bruno
{
	SceneDocument::SceneDocument(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager) :
		m_scene(scene),
		m_assetManager(assetManager)
	{
		InitializeCamera();
		InitializeGizmoService();
		InitializeSceneRenderer();
		
		m_selectionChangedHandleId = m_selectionService->SelectionChanged.connect([&](const std::vector<UUID>& selection)
		{
			Math::Matrix gizmoWorld;
			Math::Vector3 gizmoPivot;
			bool gizmoActive = m_selectionService->GetGizmoTransform(gizmoWorld, gizmoPivot);
			if (gizmoActive)
			{
				m_gizmoService->SetGizmoPosition(gizmoPivot);
				m_gizmoService->SetGizmoWorldMatrix(gizmoWorld);
			}
			m_gizmoService->SetActive(gizmoActive);
			
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
		Math::Matrix gizmoWorld;
		Math::Vector3 gizmoPivot;
		bool gizmoActive = m_selectionService->GetGizmoTransform(gizmoWorld, gizmoPivot);
		if (gizmoActive)
		{
			m_gizmoService->SetGizmoPosition(gizmoPivot);
			m_gizmoService->SetGizmoWorldMatrix(gizmoWorld);
		}
		m_gizmoService->SetActive(gizmoActive);
		
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
		
		m_gizmoService = std::make_shared<GizmoService>(device, m_camera);
		m_gizmoService->Initialize();
		m_gizmoService->SetTranslationCallback([&](const Math::Vector3& newPosition)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				// Usamos 'patch' para que EnTT dispare el evento 'on_update<TransformComponent>'
				entity.Patch<TransformComponent>([this, entity, &newPosition](auto& transform) 
				{
					Math::Matrix parentWorldMatrix = Math::Matrix::Identity;
		            
					// 1. VALIDAR SI REALMENTE TIENE PADRE
					Entity parent = entity.GetParent();
					if (parent)
					{
						parentWorldMatrix = m_scene->GetWorldSpaceMatrix(parent);
					}
		            
					Math::Matrix inverseTransform;
					parentWorldMatrix.Invert(inverseTransform);
		            
					// 2. Transform (Punto) aplica rotación, escala y traslación inversa
					// Esto convierte perfectamente la coordenada absoluta 'newPosition' al espacio local
					transform.Position = Math::Vector3::Transform(newPosition, inverseTransform);
					
					// 3. ¡VITAL PARA LA NUEVA ARQUITECTURA!
					// Marcamos el transform como sucio para que el TransformSystem 
					// recalcule las matrices World/Local de esta entidad y de TODOS sus hijos en el próximo frame.
					transform.IsDirty = true;
				});
			}
		});
		m_gizmoService->SetRotationCallback([&](const Math::Quaternion& delta)
		{
			for (auto& uuid : m_selectionService->GetSelections())
			{
				Entity entity = m_scene->GetEntityWithUUID(uuid);
				if (!entity || !entity.HasComponent<TransformComponent>()) continue;

				entity.Patch<TransformComponent>([this, entity, &delta](auto& transform) 
				{
					Math::Matrix parentWorldMatrix = Math::Matrix::Identity;
					Entity parent = entity.GetParent();
					if (parent)
					{
						parentWorldMatrix = m_scene->GetWorldSpaceMatrix(parent);
					}

					// Extraemos solo la rotación del padre en espacio de mundo
					Math::Vector3 dummyScale, dummyPos;
					Math::Quaternion parentRot;
					parentWorldMatrix.Decompose(dummyScale, parentRot, dummyPos);

					Math::Quaternion invParentRot;
					parentRot.Inverse(invParentRot);

					// Matemáticas de Jerarquía DX12
					// 1. Llevamos la rotación local al Mundo: (transform.Rotation * parentRot)
					// 2. Le sumamos el delta del ratón: (* delta)
					// 3. Lo devolvemos al espacio Local: (* invParentRot)
					transform.Rotation = transform.Rotation * parentRot * delta * invParentRot;
					transform.Rotation.Normalize(); // Previene degradación de precisión flotante
					
					transform.IsDirty = true;
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
						transform.IsDirty = true;
					}
				});
			}
		});
		
		m_cameraController = std::make_shared<EditorCameraController>(m_camera, m_scene.get(), m_selectionService.get());
	}

	void SceneDocument::InitializeSceneRenderer()
	{
		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_assetManager);
	}
}
