#pragma once

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>


namespace Bruno
{
	class EditorGame;
	class SceneDocument;

	class SceneDocumentPanel : public nana::panel<true>
	{
	public:
		SceneDocumentPanel(nana::window window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument);
		~SceneDocumentPanel();

	private:
		EditorGame* m_editorGame;
		std::shared_ptr<SceneDocument> m_sceneDocument;

		nana::place m_place;
	};
}