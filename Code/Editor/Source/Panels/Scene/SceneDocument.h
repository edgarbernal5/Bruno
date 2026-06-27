#pragma once

#include <memory>
#include <Bruno/Core/Events/Event.h>

#include <Bruno/Renderer/Camera.h>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	namespace DX
	{
		class GizmoService;
	}
	class SceneRenderer;
	class Scene;
	class Entity;
	class Model;

	class SceneHierarchy;
	class SelectionService;
	class GizmoService;
	class EditorAssetManager;

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
		std::shared_ptr<SceneHierarchy> GetSceneHierarchy() const { return m_sceneHierarchy; }

		Camera& GetCamera() { return m_camera; }
		std::shared_ptr<GizmoService> GetGizmoService() { return m_gizmoService; }
		std::shared_ptr<DX::GizmoService> GetDXGizmoService() { return m_dxGizmoService; }
		std::shared_ptr<SelectionService> GetSelectionService() { return m_selectionService; }
		EditorAssetManager* GetAssetManager() const { return m_assetManager; }
		SceneRenderer* GetSceneRenderer() const { return m_sceneRenderer.get(); }
		
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
		std::shared_ptr<DX::GizmoService> m_dxGizmoService;
		std::shared_ptr<SceneHierarchy> m_sceneHierarchy;

		EventHandlerId m_selectionChangedHandleId{ 0 };
	};
}