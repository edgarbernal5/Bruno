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
	SceneDocumentPanel::SceneDocumentPanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument) :
		Berta::Panel(window),
		m_editorGame(editorGame),
		m_sceneDocument(sceneDocument)
	{
		this->SetCaption("Scene");

		m_layout.Create(this->Handle());
		////////// VIEW
		m_layout.Parse("{VerticalLayout {Dock dockRoot}}");

		//Berta::pane_info paneInfo;
		//paneInfo.show_close_button = false;
		//paneInfo.id = "pane1";
		m_sceneHierarchyPanel = std::make_unique<SceneHierarchyPanel>(*this, m_sceneDocument);

		//paneInfo.show_caption = false;
		//paneInfo.id = "pane3";
		//auto scenePanel = m_layout.add_pane<ScenePanel>(paneInfo, "pane1", Berta::dock_position::right, editorGame, m_sceneDocument);
		m_scenePanel = std::make_unique<ScenePanel>(*this, editorGame, m_sceneDocument);

		//paneInfo.id = "pane2";
		//paneInfo.show_caption = true;
		//paneInfo.caption = "Properties";
		//auto propertiesPanel = m_layout.add_pane<PropertiesPanel>(paneInfo, "pane1", Berta::dock_position::down, m_sceneDocument);
		//m_propertiesPanel = std::make_unique<PropertiesPanel>(*this, m_sceneDocument);

		m_layout.AddPaneTab("panel-hierarchy-pane", "tab-hierarchy", *m_sceneHierarchyPanel, "", Berta::DockPosition::Tab);
		m_layout.AddPaneTab("panel-pane", "tab-scene", *m_scenePanel, "panel-hierarchy-pane", Berta::DockPosition::Right);

		//this->GetEvents().Visibility.Connect([scenePanel](const Berta::ArgVisibility& arg)
		//{
		//	if (arg.IsVisible)
		//		scenePanel->show();
		//	else
		//		scenePanel->hide();
		//});

		m_layout.Apply();
	}

	SceneDocumentPanel::~SceneDocumentPanel()
	{
	}

}