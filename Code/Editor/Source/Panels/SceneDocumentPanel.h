#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>

#include <Bruno/Renderer/Camera.h>

namespace Bruno
{
	class EditorGame;
	class Scene;
	class SelectionService;
	class GizmoService;

	class SceneDocumentPanel : public nana::panel<true>
	{
	public:
		SceneDocumentPanel(nana::window window, EditorGame* editorGame, std::shared_ptr<Scene> scene);
		~SceneDocumentPanel();

	private:
		void InitializeCamera();
		void InitializeGizmoService();

		Camera m_camera;
		EditorGame* m_editorGame;
		std::shared_ptr<Scene> m_scene;

		std::shared_ptr<SelectionService>	m_selectionService;
		std::shared_ptr<GizmoService>	m_gizmoService;

		nana::place m_place;
	};
}