#pragma once

#include <memory>
#include <Bruno/Core/Events/Event.h>

#include <Bruno/Renderer/Camera.h>
#include <Bruno/Core/UUID.h>

namespace Bruno
{
	class Scene;
	class Entity;
	class Model;

	class SelectionService;
	class GizmoService;
	class AbstractAssetManager;

	enum class ActionMode
	{
		Add,
		Delete,
		Modify
	};

	class SceneDocument
	{
	public:
		SceneDocument(std::shared_ptr<Scene> scene, AbstractAssetManager* assetManager);
		~SceneDocument();

		Entity InstantiateModel(std::shared_ptr<Model> model);
		std::shared_ptr<Scene> GetScene() const { return m_scene; }

		Camera& GetCamera() { return m_camera; }
		std::shared_ptr<GizmoService> GetGizmoService() { return m_gizmoService; }
		std::shared_ptr<SelectionService> GetSelectionService() { return m_selectionService; }

		void UpdateSelection();

		Event<Entity, ActionMode> HierarchyChanged;
		Event<std::vector<UUID>> SelectionChanged;
	private:
		void InitializeCamera();
		void InitializeGizmoService();

		Camera m_camera;
		std::shared_ptr<Scene> m_scene;
		AbstractAssetManager* m_assetManager;

		std::shared_ptr<SelectionService>	m_selectionService;
		std::shared_ptr<GizmoService>	m_gizmoService;

		size_t m_selectionChangedHandleId{ 0 };
	};
}