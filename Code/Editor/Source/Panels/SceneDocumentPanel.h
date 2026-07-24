#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>


namespace Bruno
{
	class EditorGame;
	class SceneDocument;
	class SceneHierarchyPanel;
	class ScenePanel;
	class PropertiesPanel;

	class SceneDocumentPanel : public Berta::Panel
	{
	public:
		SceneDocumentPanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument);
		~SceneDocumentPanel();

	private:
		EditorGame* m_editorGame;
		std::shared_ptr<SceneDocument> m_sceneDocument;
		std::unique_ptr<SceneHierarchyPanel> m_sceneHierarchyPanel;
		std::unique_ptr<ScenePanel> m_scenePanel;
		std::unique_ptr<PropertiesPanel> m_propertiesPanel;

		Berta::Layout m_layout;
	};
}