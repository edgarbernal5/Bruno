#include "brepch.h"
#include "SceneDocumentPanel.h"

#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include "Scene/SceneDocument.h"

#include "Panels/ScenePanel.h"
#include "Panels/SceneHierarchyPanel.h"
#include "Panels/PropertiesPanel.h"
#include "Scene/SelectionService.h"
#include "Gizmos/GizmoService.h"
#include "EditorGame.h"

namespace Bruno
{
	SceneDocumentPanel::SceneDocumentPanel(nana::window window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument) :
		nana::panel<true>(window),
		m_editorGame(editorGame),
		m_sceneDocument(sceneDocument)
	{
		this->caption("Scene");

		m_place.bind(this->handle());
		////////// VIEW
		m_place.div("<dock>");

		nana::pane_info paneInfo;
		paneInfo.show_close_button = false;
		paneInfo.id = "pane1";
		auto sceneHierarchyPanel = m_place.add_pane<SceneHierarchyPanel>(paneInfo, "", nana::dock_position::right, m_sceneDocument);

		paneInfo.show_caption = false;
		paneInfo.id = "pane3";
		auto scenePanel = m_place.add_pane<ScenePanel>(paneInfo, "pane1", nana::dock_position::right, editorGame, m_sceneDocument);

		paneInfo.id = "pane2";
		paneInfo.show_caption = true;
		paneInfo.caption = "Properties";
		auto propertiesPanel = m_place.add_pane<PropertiesPanel>(paneInfo, "pane1", nana::dock_position::down, m_sceneDocument);

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

}