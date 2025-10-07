#include "brepch.h"
#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"

#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Platform/DirectX/GraphicsContext.h>
#include <Bruno/Content/ContentManager.h>
#include <Bruno/Renderer/Model.h>
#include <Bruno/Scene/Scene.h>
#include <Bruno/Renderer/SceneRenderer.h>
#include "Panels/Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "EditorGame.h"

#include <iostream>
#include <Bruno/Core/Log.h>
#include "SceneHierarchyPanel.h"

namespace Bruno
{
	ScenePanel::ScenePanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument, const SceneSurfaceParameters& surfaceParameters) :
		//Berta::nested_form(window, Berta::appear::bald<>()),
		Berta::Panel(window),
		m_surfaceParameters(surfaceParameters),
		m_sceneDocument(sceneDocument),

		m_editorGame(editorGame)
	{
		static int idx = 0;
		idx++;
		idxx = idx;

		std::ostringstream idstr;
		idstr << "Scene id " << idxx;
		this->SetCaption(idstr.str());
		m_layout.Create(*this);

		m_scene = m_sceneDocument->GetScene();
		m_gizmoService = m_sceneDocument->GetGizmoService();
		m_selectionService = m_sceneDocument->GetSelectionService();

		m_gizmoTypeCombobox.Create(*this, false, { 0, 0, 150, 25 });
		m_gizmoTransformSpaceButton.Create(*this);

		m_layout.Parse("{VerticalLayout {HorizontalLayout {gizmoTypeComboBox}{gizmoSpaceButton} Height=25}}");

		m_layout.Attach("gizmoTypeComboBox", m_gizmoTypeCombobox);
		m_layout.Attach("gizmoSpaceButton", m_gizmoTransformSpaceButton);

		m_gizmoTypeCombobox.PushItem("None");
		m_gizmoTypeCombobox.PushItem("Translation");
		m_gizmoTypeCombobox.PushItem("Rotation");
		m_gizmoTypeCombobox.PushItem("Scale");

		m_gizmoTypeCombobox.SetSelectedIndex(1);
		m_gizmoTransformSpaceButton.SetCaption("World");

		m_gizmoTransformSpaceButton.GetEvents().Click.Connect([&](const Berta::ArgClick& click)
		{
			if (m_gizmoService->GetTransformSpace() == GizmoService::TransformSpace::World)
			{
				m_gizmoService->SetTransformSpace(GizmoService::TransformSpace::Local);
				m_gizmoTransformSpaceButton.SetCaption("World");
			}
			else
			{
				m_gizmoService->SetTransformSpace(GizmoService::TransformSpace::World);
				m_gizmoTransformSpaceButton.SetCaption("Local");
			}
		});

		//m_form = this;
		m_form = std::make_unique<Berta::NestedForm>(this->Handle(), Berta::Rectangle{}, Berta::FormStyle::Flat(), true);

		//TO-DO: ver si se puede agregar un evento al form o nested_form cuando llega un mensaje de WM_ACTIVATEAPP 
		//para luego disparar un evento y saber si el panel está activado o no. Es útil para el timer y el rendering/painting.
		/*
		1. quitar el mensaje WM_ACTIVATEAPP como trivial (bedrock_windows.cpp)
		2. crear el evento (en general_events.hpp)
		3. crear el event code (event_code.hpp)
		4. ver y analizar metodo bedrock::event_expose para emitir el evento
		*/

		// Single-thread rendering.
#ifdef BR_SINGLE_THREAD_RENDERING
		/*auto hwnd = reinterpret_cast<HWND>(m_form->NativeHandle());
		m_form->draw_through([hwnd, this]
		{
			m_timer.Tick();

			OnUpdate(m_timer);
			OnDraw();

			RECT r;
			::GetClientRect(hwnd, &r);
			::InvalidateRect(hwnd, &r, FALSE);
		});*/
		auto hwnd = m_form->NativeHandle().Handle;
		m_form->SetCustomPaintCallback([hwnd, this]()
			{
				m_timer.Tick();

				OnUpdate(m_timer);
				OnDraw();

				/*RECT r;
				::GetClientRect(hwnd, &r);
				::InvalidateRect(hwnd, &r, FALSE);*/
			}
		);
		m_form->Handle()->RenderForAttributes.AutoRefresh = true;
#endif // SINGLE_THREAD_RENDERING

		this->GetEvents().Destroy.Connect([this](const Berta::ArgDestroy& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			BR_CORE_TRACE << "destroy. panel id = " << idxx << std::endl;

			auto device = Graphics::GetDevice();
			device->WaitForIdle();

			m_isExposed = false;
			m_editorGame->RemoveScenePanel(this);
		});

		this->GetEvents().Resize.Connect([this](const Berta::ArgResize& args)
		{
			int margin = 4;
			int height = m_gizmoTypeCombobox.GetSize().Height;
			Berta::Rectangle newRect(margin, height + margin, args.NewSize.Width - margin * 2, args.NewSize.Height - height - margin * 2);
			m_form->SetArea(newRect);
		});

		this->GetEvents().Visibility.Connect([this](const Berta::ArgVisibility& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			BR_CORE_TRACE << "Expose of panel: panel id = " << idxx << ". IsVisible = " << args.IsVisible << std::endl;

			m_isExposed = args.IsVisible;
			if (m_isExposed)
				m_form->Show();
			else
				m_form->Hide();
			//TODO
			//if (args.IsVisible)
			//	this->focus();
		});

		m_form->GetEvents().EnterSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			BR_CORE_TRACE << "enter_size_move /panel id = " << idxx << std::endl;

			m_isSizingMoving = true;
		});

		m_form->GetEvents().ExitSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			BR_CORE_TRACE << "exit_size_move /panel id = " << idxx << std::endl;

			auto formSize = m_form->GetSize();
			if (m_surface)
			{
				m_surface->Resize(formSize.Width, formSize.Height);
			}
			m_sceneDocument->GetCamera().SetViewport(Math::Viewport(0.0f, 0.0f, (float)formSize.Width, (float)formSize.Height));
			m_isSizingMoving = false;
		});

		m_form->GetEvents().Resize.Connect([this](const Berta::ArgResize& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			//BR_CORE_TRACE << "Resized panel id = " << idxx << ". hwnd = " << m_form->native_handle() << ". w=" << args.width << "; h=" << args.height << std::endl;

			if (m_isSizingMoving)
				return;

			m_isResizing = true;

			if (m_surface)
			{
				m_surface->Resize(args.NewSize.Width, args.NewSize.Height);
			}
			else
			{
				SurfaceWindowParameters parameters;
				parameters.Width = args.NewSize.Width;
				parameters.Height = args.NewSize.Height;
				parameters.BackBufferFormat = m_surfaceParameters.BackBufferFormat;
				parameters.DepthBufferFormat = m_surfaceParameters.DepthBufferFormat;
				parameters.WindowHandle = m_form->NativeHandle().Handle;

				m_surface = std::make_unique<Surface>(parameters);
				m_surface->Initialize();

				//TODO: esta inicialización está acá porque depende del surface. Esto está mal, arreglarlo!
				InitializeSceneRenderer();
				InitializeGizmoService();
			}
			m_sceneDocument->GetCamera().SetViewport(Math::Viewport(0.0f, 0.0f, (float)args.NewSize.Width, (float)args.NewSize.Height));
			m_isResizing = false;
		});

		m_form->GetEvents().MouseDown.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			//BR_CORE_TRACE << "Mouse down x=" << args.pos.x << "; y=" << args.pos.y << std::endl;
			m_lastMousePosition.x = args.Position.X;
			m_lastMousePosition.y = args.Position.Y;
			m_beginMouseDownPosition = m_lastMousePosition;

			m_isGizmoing = args.ButtonState.LeftButton && m_gizmoService->BeginDrag(Math::Vector2(args.Position.X, args.Position.Y));
			//TODO
			//m_form->set_capture(true);
		});

		m_form->GetEvents().MouseMove.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			Math::Int2 currentPosition{ args.Position.X, args.Position.Y };

			if (!m_isGizmoing && !args.ButtonState.LeftButton)
			{
				m_gizmoService->OnMouseMove(Math::Vector2(args.Position.X, args.Position.Y));
			}

			if (m_isGizmoing)
			{
				m_gizmoService->Drag(Math::Vector2(args.Position.X, args.Position.Y));
			}
			else
			{
				if (args.ButtonState.LeftButton)
				{
					//if (args.alt)
					//TODO
					if (false)
					{
						m_sceneDocument->GetCamera().Rotate(currentPosition, m_lastMousePosition);
					}
					else
					{
						int dragLength = Math::Abs(m_beginMouseDownPosition.x - currentPosition.x) + Math::Abs(m_beginMouseDownPosition.y - currentPosition.y);

						if (!m_dragRectangle)
						{
							m_dragRectangle = dragLength > 2;
						}
					}
				}
				else if (args.ButtonState.MiddleButton)
				{
					m_sceneDocument->GetCamera().HandTool(currentPosition, m_lastMousePosition);
				}
				else if (args.ButtonState.RightButton)
				{
					m_sceneDocument->GetCamera().PitchYaw(currentPosition, m_lastMousePosition);
				}
			}
			
			m_lastMousePosition.x = args.Position.X;
			m_lastMousePosition.y = args.Position.Y;
		});

		m_form->GetEvents().MouseUp.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			Math::Int2 currentPosition{ args.Position.X, args.Position.Y };
			
			if (args.ButtonState.LeftButton)
			{
				if (m_isGizmoing)
				{
					m_gizmoService->EndDrag();
					m_isGizmoing = false;
				}
				else
				{
					if (m_dragRectangle)
					{

						m_dragRectangle = false;
					}
					//TODO
					//else if (!args.alt)
					if (false)
					{
						m_selectionService->SelectUnderMousePosition(m_sceneDocument->GetCamera(), currentPosition);
					}
				}
			}

			//TODO
			//m_form->release_capture();
		});

		m_form->GetEvents().MouseWheel.Connect([this](const Berta::ArgWheel& args)
		{
			float zoom = args.WheelDelta * 0.0025f;
			if (!args.IsVertical) zoom = -zoom;

			m_sceneDocument->GetCamera().Zoom(zoom);
		});

		m_form->GetEvents().KeyPressed.Connect([this](const Berta::ArgKeyboard& args)
		{
			if (args.Key == 'A')
			{
				m_sceneDocument->GetCamera().Strafe(-0.25f);
			}
			else if (args.Key == 'D')
			{
				m_sceneDocument->GetCamera().Strafe(0.25f);
			}
			else if (args.Key == 'W')
			{
				m_sceneDocument->GetCamera().Walk(0.25f);
			}
			else if (args.Key == 'S')
			{
				m_sceneDocument->GetCamera().Walk(-0.25f);
			}
		});

		m_gizmoTypeCombobox.GetEvents().Selected.Connect([this](const Berta::ArgComboBox& acmb) mutable
		{
			//BR_CORE_TRACE << "Gizmo type selected: " << acmb.widget.option() << std::endl;

			if (m_gizmoService)
			{
				m_gizmoService->SetGizmoType(static_cast<GizmoService::GizmoType>(m_gizmoTypeCombobox.GetSelectedIndex()));
			}
			m_gizmoTransformSpaceButton.SetEnabled(m_gizmoTypeCombobox.GetSelectedIndex() < 3);
		});

		InitializeGraphicsContext();

		editorGame->AddScenePanel(this);
		m_form->Show();
		m_timer.Reset();
		m_isExposed = true;
	}

	ScenePanel::~ScenePanel()
	{
#ifndef BR_SINGLE_THREAD_RENDERING
		std::lock_guard lock{ m_mutex };
#endif
		BR_CORE_TRACE << "destructor panel id = " << idxx << std::endl;

		auto device = Graphics::GetDevice();
		device->WaitForIdle();

		m_isExposed = false;
		m_editorGame->RemoveScenePanel(this);
	}

	void ScenePanel::OnUpdate(const GameTimer& timer)
	{
		//BR_CORE_TRACE << "Paint panel. id = " << idxx << ". delta time = " << timer.GetDeltaTime() << std::endl;

		if (!m_isExposed || m_isResizing || m_isSizingMoving || !m_surface)
			return;

		auto device = Graphics::GetDevice();
		device->BeginFrame();

		UpdateCBs(timer);
		m_gizmoService->Update();
	}

	void ScenePanel::OnDraw()
	{
		if (!m_isExposed || m_isResizing || m_isSizingMoving || !m_surface)
			return;

		auto device = Graphics::GetDevice();
		Math::Color clearColor{ 1.0f, 1.0f, 0.0f, 1.0f };
		if (idxx == 2) {
			clearColor.R(0.5f);
		}
		else if (idxx == 3) {
			clearColor.R(0.25f);
			clearColor.G(0.0f);
		}
		else if (idxx == 4) {
			clearColor.R(0.0f);
			clearColor.G(0.25f);
			clearColor.B(0.5f);
		}
		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		m_graphicsContext->Reset();
		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_graphicsContext->FlushBarriers();

		m_graphicsContext->ClearRenderTarget(backBuffer, clearColor);
		m_graphicsContext->ClearDepthStencilTarget(depthBuffer, 1.0f, 0);

		m_graphicsContext->SetViewport(m_surface->GetViewport());
		m_graphicsContext->SetScissorRect(m_surface->GetScissorRect());

		m_sceneRenderer->OnRender(m_graphicsContext.get());

		m_gizmoService->Render(m_graphicsContext.get(), m_surface.get());

		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_graphicsContext->FlushBarriers();

		device->SubmitContextWork(*m_graphicsContext);

		device->EndFrame();
		device->Present(m_surface.get());
	}

	bool ScenePanel::IsEnabled()
	{
#ifndef BR_SINGLE_THREAD_RENDERING
		std::lock_guard lock{ m_mutex };
#endif

		return (m_isExposed && !m_isResizing && !m_isSizingMoving);
	}

	void ScenePanel::InitializeGizmoService()
	{
		m_gizmoService->SetGizmoType(static_cast<GizmoService::GizmoType>(m_gizmoTypeCombobox.GetSelectedIndex()));
		m_gizmoService->SetTransformSpace(m_gizmoTransformSpaceButton.GetCaption() == "Local" ? GizmoService::TransformSpace::World : GizmoService::TransformSpace::Local);
	}

	void ScenePanel::InitializeGraphicsContext()
	{
		GraphicsDevice* device = Graphics::GetDevice();
		m_graphicsContext = std::make_unique<GraphicsContext>(*device);
	}

	void ScenePanel::InitializeSceneRenderer()
	{
		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_surface.get(), m_editorGame->GetAssetManager());
	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer, m_sceneDocument->GetCamera());
	}
}
