#pragma once

#include <Berta/Controls/Panel.h>
#include <Berta/GUI/Layout.h>


namespace Bruno
{
	class EditorGame;
	class SceneDocument;

	class SceneDocumentPanel : public Berta::Panel
	{
	public:
		SceneDocumentPanel(Berta::Window window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument);
		~SceneDocumentPanel();

	private:
		EditorGame* m_editorGame;
		std::shared_ptr<SceneDocument> m_sceneDocument;

		Berta::Layout m_place;
	};
}