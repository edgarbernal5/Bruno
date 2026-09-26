#pragma once

#include <memory>
#include <Bruno/Core/Events/Event.h>

#include <Bruno/Renderer/Camera.h>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	class ShadowSystem;
	class Shader;
	class CullingSystem;
	class SceneRenderer;
	class Scene;
	class Entity;
	class Model;

	class SelectionService;
	class EditorAssetManager;
	class GizmoService;
	class EditorCameraController;

	enum class ActionMode
	{
		Add,
		Delete,
		Modify
	};

	class SceneDocument
	{
	public:
		SceneDocument(std::shared_ptr<Scene> scene, EditorAssetManager* assetManager);
		~SceneDocument();

		void InstantiateModel(std::shared_ptr<Model> model);
		std::shared_ptr<Scene> GetScene() const { return m_scene; }
		
		Camera& GetCamera() { return m_camera; }
		std::shared_ptr<GizmoService> GetGizmoService() { return m_gizmoService; }
		std::shared_ptr<SelectionService> GetSelectionService() { return m_selectionService; }
		EditorAssetManager* GetAssetManager() const { return m_assetManager; }
		SceneRenderer* GetSceneRenderer() const { return m_sceneRenderer.get(); }
		EditorCameraController* GetCameraController() const { return m_cameraController.get(); }
		CullingSystem* GetCullingSystem() const { return m_cullingSystem.get(); }
		ShadowSystem* GetShadowSystem() const { return m_shadowSystem.get(); }
		
		void UpdateSelection();

		Event<Entity, ActionMode> HierarchyChanged;
		Event<std::vector<UUID>> SelectionChanged;
	private:
		void InitializeCamera();
		void InitializeGizmoService();
		void InitializeSceneRenderer();

		Camera m_camera;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<SceneRenderer> m_sceneRenderer;
		EditorAssetManager* m_assetManager;

		std::shared_ptr<SelectionService> m_selectionService;
		std::shared_ptr<GizmoService> m_gizmoService;
		std::shared_ptr<EditorCameraController> m_cameraController;
		std::shared_ptr<CullingSystem> m_cullingSystem;
		std::shared_ptr<ShadowSystem> m_shadowSystem;

		EventHandlerId m_selectionChangedHandleId{ 0 };
	};
}