#include "brepch.h"
#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"

#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Platform/DirectX/GraphicsContext.h>
#include <Bruno/Platform/DirectX/Queue.h>
#include <Bruno/Platform/DirectX/Surface_Gem.h>
#include <Bruno/Scene/Scene.h>
#include <Bruno/Renderer/SceneRenderer.h>
#include "Panels/Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "EditorGame.h"

#include <iostream>
#include <Bruno/Core/Log.h>
#include "SceneHierarchyPanel.h"
#include "Bruno/Platform/DirectX/GraphicsContext_Gem.h"
#include "Bruno/Platform/DirectX/Shader_Gem.h"
#include "Gizmos/GizmoService_Gem.h"

namespace Bruno
{
	ScenePanel::ScenePanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument, const SceneSurfaceParameters& surfaceParameters) :
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

		m_layout.Parse("{VerticalLayout {HorizontalLayout {gizmoTypeComboBox}{gizmoSpaceButton} Height=25} {renderForm}}");

		m_layout.Attach("gizmoTypeComboBox", m_gizmoTypeCombobox);
		m_layout.Attach("gizmoSpaceButton", m_gizmoTransformSpaceButton);

		m_gizmoTypeCombobox.PushBack("None");
		m_gizmoTypeCombobox.PushBack("Translation");
		m_gizmoTypeCombobox.PushBack("Rotation");
		m_gizmoTypeCombobox.PushBack("Scale");

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
		m_form = std::make_unique<Berta::NestedForm>(this->Handle(), Berta::Rectangle{0,0,100,100}, Berta::FormStyle::Flat(), true);
		m_layout.Attach("renderForm", *m_form);
		
		m_dxViewport.Height=100;
		m_dxViewport.Width=100;
		m_dxViewport.TopLeftX	=0;
		m_dxViewport.TopLeftY	=0;
		m_dxViewport.MinDepth	=D3D12_MIN_DEPTH;
		m_dxViewport.MaxDepth	=D3D12_MAX_DEPTH;
		m_dxDevice = Graphics::GetDXDevice();
		
		DX::SurfaceWindowParameters parameters;
		parameters.Width = 100;
		parameters.Height = 100;
		parameters.BackBufferFormat = m_surfaceParameters.BackBufferFormat;
		parameters.DepthBufferFormat = m_surfaceParameters.DepthBufferFormat;
		parameters.WindowHandle = m_form->NativeHandle().Handle;
		
		m_dxSurface = std::make_unique<DX::Surface>(*m_dxDevice, parameters);
		m_commandQueue = &m_dxDevice->GetDirectCommandQueue();
		
		InitializeSceneRenderer();
		InitializeGizmoService();
		m_srvHeap = m_dxDevice->GetSRVDescriptorAllocator().GetHeap();
		
		// Single-thread rendering.
#ifdef BR_SINGLE_THREAD_RENDERING
		
		m_form->SetCustomPaintCallback([this]()
		{
			m_timer.Tick();
			
			if (!m_isVisible || m_isResizing || m_isSizingMoving)
			{
				return;
			}
			
			// 1. Preguntarle al SwapChain en qué frame (0 o 1) estamos trabajando hoy
			uint32_t frameIndex = m_dxSurface->GetCurrentBackBufferIndex();

			//BT_CORE_TRACE << "Scene / delta time = " << m_timer.GetDeltaTime() << ". frameid= "<< frameIndex <<std::endl;
			
			// 2. Pedirle a nuestra cola el "lápiz" (CommandList). 
			// Magia: Esto automáticamente espera si la GPU sigue ocupada con este frame.
			auto commandList = m_commandQueue->GetCommandList(frameIndex);
			auto allocator = m_commandQueue->GetAllocator(frameIndex);
			DX::GraphicsContext context(*m_dxDevice, commandList.Get(), allocator.Get());
			
			// 3. Extraer la textura real y su descriptor
			auto backBuffer = m_dxSurface->GetCurrentBackBuffer();
			auto rtvHandle = m_dxSurface->GetCurrentRenderTargetView();

			// ------------------------------------------------------------------
			// FASE DE TRANSICIÓN: PRESENT -> RENDER_TARGET
			// ------------------------------------------------------------------
			context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			// ------------------------------------------------------------------
			// FASE DE DIBUJO
			// ------------------------------------------------------------------
			// Un azul oscuro/grisáceo muy estilo editor AAA (R, G, B, A)
			//const float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; 
			const float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
			auto dsvHandle = m_dxSurface->GetDepthBufferView();
			
			// Limpiar la pantalla
			context.ClearRenderTarget(rtvHandle, clearColor);
			context.ClearDepth(dsvHandle, 1.0f, 0);
			context.SetRenderTargets(1, &rtvHandle, &dsvHandle);
			
			// Setear SRV Heaps (Indispensable para que la GPU encuentre la textura)
			context.SetDescriptorHeaps(&m_srvHeap, 1);
			
			// Configurar Viewport y Scissor Test explícitamente en este frame
			context.SetViewport(m_dxViewport);
			context.SetScissorRect(m_scissorRect);
			
			m_sceneRenderer->OnRender(&context, m_sceneDocument->GetCamera(), frameIndex);
			Math::Matrix viewProj = m_sceneDocument->GetCamera().GetViewProjection();
    
			m_dxGizmoService->Render(&context, viewProj);
			// ------------------------------------------------------------------
			// FASE DE TRANSICIÓN: RENDER_TARGET -> PRESENT
			// ------------------------------------------------------------------
			context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			// 4. Cerrar el lápiz y enviarlo a la GPU para que lo ejecute
			m_commandQueue->ExecuteCommandList(commandList, frameIndex);

			// 5. Intercambiar los buffers y mostrar en pantalla (VSync activado por ahora)
			m_dxSurface->Present(true);
		});
		
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

			m_isVisible = false;
			m_editorGame->RemoveScenePanel(this);
		});

		this->GetEvents().Visibility.Connect([this](const Berta::ArgVisibility& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			BR_CORE_TRACE << "Expose of panel: panel id = " << idxx << ". IsVisible = " << args.IsVisible << std::endl;

			m_isVisible = args.IsVisible;
			if (m_isVisible)
				m_form->Show();
			else
				m_form->Hide();
			
			if (args.IsVisible)
				this->Focus();
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
			if (m_dxSurface)
			{
				m_dxSurface->Resize(formSize.Width, formSize.Height);
			}
			m_dxViewport.Height = formSize.Height;
			m_dxViewport.Width = formSize.Width;
			m_scissorRect = { 0, 0, static_cast<LONG>(formSize.Width), static_cast<LONG>(formSize.Height) };
			
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
			m_dxViewport.Height = args.NewSize.Height;
			m_dxViewport.Width = args.NewSize.Width;
			m_scissorRect = { 0, 0, static_cast<LONG>(args.NewSize.Width), static_cast<LONG>(args.NewSize.Height) };
			m_dxSurface->Resize(args.NewSize.Width, args.NewSize.Height);

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

			if (args.ButtonState.LeftButton)
			{
				m_isGizmoing = m_dxGizmoService->BeginDrag(Math::Vector2(args.Position.X, args.Position.Y));
				std::cout << "is gizmoing: " << m_isGizmoing << std::endl;
				if (!m_isGizmoing)
				{
					//Entity selectedEntity = m_scene->Raycast(mousePosition);
					//m_selectionService->SetSelection(selectedEntity);
				}
			}
			m_form->Capture(false);
		});

		m_form->GetEvents().MouseMove.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			Math::Int2 currentPosition{ args.Position.X, args.Position.Y };

			if (!m_isGizmoing && !args.ButtonState.LeftButton)
			{
				//m_dxGizmoService->OnMouseMove(Math::Vector2(args.Position.X, args.Position.Y));
			}

			if (m_dxGizmoService->IsDragging())
			//if (m_isGizmoing)
			{
				m_dxGizmoService->Drag(Math::Vector2(args.Position.X, args.Position.Y));
			}
			else
			{
				if (args.ButtonState.LeftButton)
				{
					if (args.AltPressed)
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
					m_dxGizmoService->EndDrag();
					m_isGizmoing = false;
				}
				else
				{
					if (m_dragRectangle)
					{

						m_dragRectangle = false;
					}
					else if (!args.AltPressed)
					{
						m_selectionService->SelectUnderMousePosition(m_sceneDocument->GetCamera(), currentPosition);
					}
				}
			}

			m_form->ReleaseCapture();
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
			if (!acmb.SelectedIndex.has_value())
				return;
			
			auto index = acmb.SelectedIndex.value();
			if (m_gizmoService)
			{
				m_gizmoService->SetGizmoType(static_cast<GizmoService::GizmoType>(index));
			}
			m_gizmoTransformSpaceButton.SetEnabled(index < 3);
		});

		InitializeGraphicsContext();

		editorGame->AddScenePanel(this);
		m_form->Show();
		m_timer.Reset();
		m_isVisible = true;
	}

	ScenePanel::~ScenePanel()
	{
#ifndef BR_SINGLE_THREAD_RENDERING
		std::lock_guard lock{ m_mutex };
#endif
		BR_CORE_TRACE << "destructor panel id = " << idxx << std::endl;

		auto device = Graphics::GetDevice();
		device->WaitForIdle();

		m_isVisible = false;
		m_editorGame->RemoveScenePanel(this);
	}

	void ScenePanel::OnUpdate(const GameTimer& timer)
	{
		//BR_CORE_TRACE << "Paint panel. id = " << idxx << ". delta time = " << timer.GetDeltaTime() << std::endl;

		if (!m_isVisible || m_isResizing || m_isSizingMoving || !m_surface)
			return;

		auto device = Graphics::GetDevice();
		device->BeginFrame();

		//UpdateCBs(timer);
		//m_gizmoService->Update();
	}

	void ScenePanel::OnDraw()
	{
		if (!m_isVisible || m_isResizing || m_isSizingMoving || !m_surface)
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

		//m_sceneRenderer->OnRender(m_graphicsContext.get());

		//m_gizmoService->Render(m_graphicsContext.get(), m_surface.get());

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

		return (m_isVisible && !m_isResizing && !m_isSizingMoving);
	}

	void ScenePanel::InitializeGizmoService()
	{
		m_gizmoService->SetGizmoType(static_cast<GizmoService::GizmoType>(m_gizmoTypeCombobox.GetSelectedIndex().value()));
		m_gizmoService->SetTransformSpace(m_gizmoTransformSpaceButton.GetCaption() == "Local" ? GizmoService::TransformSpace::World : GizmoService::TransformSpace::Local);
		
		m_dxGizmoService = m_sceneDocument->GetDXGizmoService();
	}

	void ScenePanel::InitializeGraphicsContext()
	{
		GraphicsDevice* device = Graphics::GetDevice();
		//m_graphicsContext = std::make_unique<GraphicsContext>(*device);
	}

	void ScenePanel::InitializeSceneRenderer()
	{
		m_sceneRenderer = m_sceneDocument->GetSceneRenderer();
	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer, m_sceneDocument->GetCamera());
	}
}
