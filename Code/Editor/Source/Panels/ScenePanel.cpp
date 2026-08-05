#include "brepch.h"
#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"

#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Platform/DirectX/GraphicsContext.h>
#include <Bruno/Platform/DirectX/CommandQueue.h>
#include <Bruno/Scene/Scene.h>
#include <Bruno/Renderer/SceneRenderer.h>
#include "Panels/Scene/SelectionService.h"
#include "Scene/SceneDocument.h"
#include "EditorGame.h"

#include <iostream>
#include <Bruno/Core/Log.h>

#include "SceneHierarchyPanel.h"
#include "Bruno/Platform/DirectX/DynamicAllocation.h"
#include "Bruno/Platform/DirectX/ShaderCompiler.h"
#include "Bruno/Scene/Systems/TransformSystem.h"
#include "Gizmos/GizmoService.h"
#include "Gizmos/CameraGizmo.h"
#include "Scene/EditorCameraController.h"

namespace Bruno
{
	ScenePanel::ScenePanel(Berta::Window* window, EditorGame* editorGame, std::shared_ptr<SceneDocument> sceneDocument, const SceneSurfaceParameters& surfaceParameters) :
		Berta::Panel(window),
		m_surfaceParameters(surfaceParameters),
		m_sceneDocument(sceneDocument),

		m_editorGame(editorGame)
	{
		std::ostringstream idstr;
		idstr << "Scene id " << 0;
		this->SetCaption(idstr.str());
		m_layout.Create(*this);

		m_scene = m_sceneDocument->GetScene();
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
			if (m_gizmoService->GetTransformSpace() == TransformSpace::World)
			{
				m_gizmoService->SetTransformSpace(TransformSpace::Local);
				m_gizmoTransformSpaceButton.SetCaption("World");
			}
			else
			{
				m_gizmoService->SetTransformSpace(TransformSpace::World);
				m_gizmoTransformSpaceButton.SetCaption("Local");
			}
		});

		//m_form = this;
		m_form = std::make_unique<Berta::NestedForm>(this->Handle(), Berta::Rectangle{0,0,100,100}, Berta::FormStyle::Flat(), true);
		m_layout.Attach("renderForm", *m_form);
		
		m_viewport.height = 100;
		m_viewport.width = 100;
		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.minDepth = D3D12_MIN_DEPTH;
		m_viewport.maxDepth = D3D12_MAX_DEPTH;
		m_device = Graphics::GetDevice();
		
		SurfaceWindowParameters parameters;
		parameters.Width = 100;
		parameters.Height = 100;
		parameters.BackBufferFormat = m_surfaceParameters.BackBufferFormat;
		parameters.DepthBufferFormat = m_surfaceParameters.DepthBufferFormat;
		parameters.WindowHandle = m_form->NativeHandle().Handle;
		
		m_surface = std::make_unique<Surface>(*m_device, parameters);
		m_commandQueue = &m_device->GetDirectCommandQueue();
		
		InitializeSceneRenderer();
		InitializeGizmoService();
		m_srvHeap = m_device->GetSRVDescriptorAllocator().GetHeap();

		for (int i = 0; i < m_dynamicAllocators.size(); ++i)
		{
			m_dynamicAllocators[i] = std::make_unique<LinearAllocator>(*m_device);
		}
		
		InitializeMarquee();
		
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
			uint32_t frameIndex = m_surface->GetCurrentBackBufferIndex();

			//BT_CORE_TRACE << "Scene / delta time = " << m_timer.GetDeltaTime() << ". frameid= "<< frameIndex <<std::endl;
			
			// 2. Pedirle a nuestra cola el "lápiz" (CommandList). 
			// Magia: Esto automáticamente espera si la GPU sigue ocupada con este frame.
			auto commandList = m_commandQueue->GetCommandList(frameIndex);
			auto allocator = m_commandQueue->GetAllocator(frameIndex);
			auto dynamicAllocator = m_dynamicAllocators[frameIndex].get();
			GraphicsContext context(*m_device, commandList.Get(), allocator.Get(), dynamicAllocator);
			
			dynamicAllocator->Reset();
			
			// 3. Extraer la textura real y su descriptor
			auto backBuffer = m_surface->GetCurrentBackBuffer();
			auto rtvHandle = m_surface->GetCurrentRenderTargetView();

			// ------------------------------------------------------------------
			// FASE DE TRANSICIÓN: PRESENT -> RENDER_TARGET
			// ------------------------------------------------------------------
			context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			
			// ------------------------------------------------------------------
			// FASE DE DIBUJO
			// ------------------------------------------------------------------
			// Un azul oscuro/grisáceo muy estilo editor AAA (R, G, B, A)
			const float clearColor[] = { 0.10f, 0.014f, 0.16f, 1.0f }; 
			//const float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
			auto dsvHandle = m_surface->GetDepthBufferView();
			
			// Limpiar la pantalla
			context.ClearRenderTarget(rtvHandle, clearColor);
			context.ClearDepth(dsvHandle, 1.0f, 0);
			context.SetRenderTargets(1, &rtvHandle, &dsvHandle);
			
			// Setear SRV Heaps (Indispensable para que la GPU encuentre la textura)
			context.SetDescriptorHeaps(&m_srvHeap, 1);
			
			SetCameraGizmoViewport();
			
			// Configurar Viewport y Scissor Test explícitamente en este frame
			context.SetViewport(m_viewport);
			context.SetScissorRect(m_scissorRect);
			
			Math::Matrix viewProj = m_sceneDocument->GetCamera().GetViewProjection();
			Math::Matrix gizmoWorld;
			Math::Vector3 gizmoPivot;
			
			TransformSystem::Update(m_scene.get());
			m_sceneRenderer->OnRender(&context, m_sceneDocument->GetCamera(), frameIndex);
			
			if (m_selectionService->GetGizmoTransform(gizmoWorld, gizmoPivot))
			{
				m_gizmoService->SetGizmoPosition(gizmoPivot);
				m_gizmoService->SetGizmoWorldMatrix(gizmoWorld);
			}
			
			if (m_marqueeInteraction.m_dragRectangle)
			{
				RenderMarquee(context, m_marqueeInteraction.m_ndcMin, m_marqueeInteraction.m_ndcMax);
			}
			m_gizmoService->Update();
			m_gizmoService->BuildGeometry(frameIndex);
			m_gizmoService->Render(&context, frameIndex, viewProj);
			
			m_cameraGizmo->BuildCameraGizmoGeometry(frameIndex);
			m_cameraGizmo->RenderCameraGizmo(&context, frameIndex, m_viewport);
			
			// ------------------------------------------------------------------
			// FASE DE TRANSICIÓN: RENDER_TARGET -> PRESENT
			// ------------------------------------------------------------------
			context.TransitionResource(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			// 4. Cerrar el lápiz y enviarlo a la GPU para que lo ejecute
			m_commandQueue->ExecuteCommandList(commandList, frameIndex);

			// 5. Intercambiar los buffers y mostrar en pantalla (VSync activado por ahora)
			m_surface->Present(true);
		});
		
		m_form->Handle()->RenderForAttributes.AutoRefresh = true;
#endif // SINGLE_THREAD_RENDERING

		this->GetEvents().Destroy.Connect([this](const Berta::ArgDestroy& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			
			auto device = Graphics::GetDevice();
			device->Flush();

			m_isVisible = false;
			m_editorGame->RemoveScenePanel(this);
		});

		this->GetEvents().Visibility.Connect([this](const Berta::ArgVisibility& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif

			m_isVisible = args.IsVisible;
			m_isVisible ? m_form->Show() : m_form->Hide();
			
			if (args.IsVisible)
			{
				this->Focus();
			}
		});

		m_form->GetEvents().EnterSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif

			m_isSizingMoving = true;
		});

		m_form->GetEvents().ExitSizeMove.Connect([this](const Berta::ArgSizeMove& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif

			auto formSize = m_form->GetSize();
			if (m_surface)
			{
				m_surface->Resize(formSize.Width, formSize.Height);
			}
			m_viewport.height = formSize.Height;
			m_viewport.width = formSize.Width;
			m_scissorRect = { 0, 0, static_cast<LONG>(formSize.Width), static_cast<LONG>(formSize.Height) };
			
			m_sceneDocument->GetCamera().SetViewport(Math::Viewport(0.0f, 0.0f, static_cast<float>(formSize.Width), static_cast<float>(formSize.Height)));
			m_isSizingMoving = false;
		});

		m_form->GetEvents().Resize.Connect([this](const Berta::ArgResize& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			//BR_CORE_TRACE << "Resized panel id = " << idxx << ". hwnd = " << m_form->native_handle() << ". w=" << args.width << "; h=" << args.height << std::endl;

			if (m_isSizingMoving)
			{
				return;
			}

			m_isResizing = true;
			m_viewport.height = args.NewSize.Height;
			m_viewport.width = args.NewSize.Width;
			m_scissorRect = { 0, 0, static_cast<LONG>(args.NewSize.Width), static_cast<LONG>(args.NewSize.Height) };
			m_surface->Resize(args.NewSize.Width, args.NewSize.Height);

			m_sceneDocument->GetCamera().SetViewport(Math::Viewport(0.0f, 0.0f, static_cast<float>(args.NewSize.Width), static_cast<float>(args.NewSize.Height)));
			m_isResizing = false;
		});

		m_form->GetEvents().MouseDown.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			m_form->Capture(false);
			
			//BR_CORE_TRACE << "Mouse down x=" << args.pos.x << "; y=" << args.pos.y << std::endl;
			m_lastMousePosition.x = args.Position.X;
			m_lastMousePosition.y = args.Position.Y;
			m_beginMouseDownPosition = m_lastMousePosition;

			if (args.ButtonState.LeftButton)
			{
				if (m_cameraGizmo->OnMouseDown(Math::Vector2(args.Position.X, args.Position.Y)))
				{
					return;
				}
				
				m_isGizmoing = m_gizmoService->BeginDrag(Math::Vector2(args.Position.X, args.Position.Y));
				
				std::cout << "is gizmoing: " << m_isGizmoing << std::endl;
				if (m_isGizmoing)
				{
					return;
				}
				
				m_gizmoService->SetActive(false);
				
				//Entity selectedEntity = m_scene->Raycast(mousePosition);
				//m_selectionService->SetSelection(selectedEntity);
			}
		});

		m_form->GetEvents().MouseMove.Connect([this](const Berta::ArgMouse& args)
		{
#ifndef BR_SINGLE_THREAD_RENDERING
			std::lock_guard lock{ m_mutex };
#endif
			Math::Int2 currentPosition{ args.Position.X, args.Position.Y };

			if (m_cameraGizmo->OnMouseMove(Math::Vector2(args.Position.X, args.Position.Y)))
			{
				return;
			}
			m_gizmoService->SetSnapEnabled(args.ShiftPressed);
			m_gizmoService->SetPrecisionModeEnabled(args.CtrlPressed);
			
			if (m_gizmoService->IsDragging())
			{
				m_gizmoService->Drag(Math::Vector2(args.Position.X, args.Position.Y));
			}
			else
			{
				m_gizmoService->OnMouseMove(Math::Vector2(args.Position.X, args.Position.Y));
				
				if (args.ButtonState.LeftButton)
				{
					if (args.AltPressed)
					{
						m_sceneDocument->GetCamera().Rotate(currentPosition, m_lastMousePosition);
					}
					else
					{
						int dragLength = Math::Abs(m_beginMouseDownPosition.x - currentPosition.x) + Math::Abs(m_beginMouseDownPosition.y - currentPosition.y);

						if (!m_marqueeInteraction.m_dragRectangle)
						{
							m_marqueeInteraction.m_dragRectangle = dragLength > 2;
						}
						if (m_marqueeInteraction.m_dragRectangle)
						{
							// Dimensiones de tu viewport (resolución del ScenePanel)
							float screenWidth = static_cast<float>(m_viewport.width);
							float screenHeight = static_cast<float>(m_viewport.height);

							// Convertir de Píxeles a NDC [-1, 1]
							Math::Vector2 ndcMin;
							Math::Vector2 ndcMax;

							// Calculamos los min/max por si el usuario arrastró el mouse de derecha a izquierda
							float minX = std::min<int>(m_beginMouseDownPosition.x, args.Position.X);
							float maxX = std::max<int>(m_beginMouseDownPosition.x, args.Position.X);
							float minY = std::min<int>(m_beginMouseDownPosition.y, args.Position.Y);
							float maxY = std::max<int>(m_beginMouseDownPosition.y, args.Position.Y);

							ndcMin.x =  (minX / screenWidth) * 2.0f - 1.0f;
							ndcMax.x =  (maxX / screenWidth) * 2.0f - 1.0f;

							// Invertimos Y para DirectX (Mouse coord Y=0 es arriba, NDC Y=1 es arriba)
							ndcMin.y = -((maxY / screenHeight) * 2.0f - 1.0f); 
							ndcMax.y = -((minY / screenHeight) * 2.0f - 1.0f);

							m_marqueeInteraction.m_ndcMin = ndcMin;
							m_marqueeInteraction.m_ndcMax = ndcMax;
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
			m_form->ReleaseCapture();
			
			Math::Int2 currentPosition{ args.Position.X, args.Position.Y };
			
			if (args.ButtonState.LeftButton)
			{
				if (m_cameraGizmo->OnMouseUp(Math::Vector2(args.Position.X, args.Position.Y)))
				{
					return;
				}
				if (m_gizmoService->IsDragging())
				{
					m_gizmoService->EndDrag();
					m_isGizmoing = false;
				}
				else
				{
					if (m_marqueeInteraction.m_dragRectangle)
					{
						m_selectionService->SelectEntitiesInRect(m_sceneDocument->GetCamera(), m_marqueeInteraction.m_ndcMin, m_marqueeInteraction.m_ndcMax);
						
						m_marqueeInteraction.m_dragRectangle = false;
					}
					else if (!args.AltPressed)
					{
						m_selectionService->SelectUnderMousePosition(m_sceneDocument->GetCamera(), currentPosition);
					}
				}
			}
		});

		m_form->GetEvents().MouseWheel.Connect([this](const Berta::ArgWheel& args)
		{
			float zoom = args.WheelDelta * 0.0025f;
			if (!args.IsVertical)
			{
				zoom = -zoom;
			}
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
			else if (args.Key == 'F')
			{
				m_sceneDocument->GetCameraController()->FocusOnSelection();
			}
		});

		m_gizmoTypeCombobox.GetEvents().Selected.Connect([this](const Berta::ArgComboBox& acmb) mutable
		{
			//BR_CORE_TRACE << "Gizmo type selected: " << acmb.widget.option() << std::endl;
			if (!acmb.SelectedIndex.has_value())
			{
				return;
			}
			
			auto index = acmb.SelectedIndex.value();
			if (m_gizmoService)
			{
				m_gizmoService->SetGizmoType(static_cast<GizmoType>(index));
			}
			m_gizmoTransformSpaceButton.SetEnabled(index < 3);
		});
		
		SetCameraGizmoViewport();
		editorGame->AddScenePanel(this);
		
		m_form->Show();
		m_timer.Reset();
		m_isVisible = true;
		
		m_form->Focus();
	}

	ScenePanel::~ScenePanel()
	{
#ifndef BR_SINGLE_THREAD_RENDERING
		std::lock_guard lock{ m_mutex };
#endif

		auto device = Graphics::GetDevice();
		device->Flush();

		m_isVisible = false;
		m_editorGame->RemoveScenePanel(this);
	}

	void ScenePanel::OnUpdate(const GameTimer& timer)
	{
		
	}

	void ScenePanel::OnDraw()
	{
		
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
		m_gizmoService = m_sceneDocument->GetGizmoService();
		m_gizmoService->SetGizmoType(static_cast<GizmoType>(m_gizmoTypeCombobox.GetSelectedIndex().value()));
		m_gizmoService->SetTransformSpace(m_gizmoTransformSpaceButton.GetCaption() == "Local" ? TransformSpace::World : TransformSpace::Local);
		
		auto device = Graphics::GetDevice();
		
		m_cameraGizmo = std::make_unique<CameraGizmo>(device, m_sceneDocument->GetCamera());
		m_cameraGizmo->Initialize();
	}

	void ScenePanel::InitializeSceneRenderer()
	{
		m_sceneRenderer = m_sceneDocument->GetSceneRenderer();
	}

	void ScenePanel::InitializeMarquee()
	{
		ShaderCompiler compiler; 

		// 1. Compilar Shaders
		auto vertexShaderByteCode = compiler.CompileFromFile(L"Shaders/Marquee.hlsl", L"VS", L"vs_6_0");
		auto pixelShaderByteCode  = compiler.CompileFromFile(L"Shaders/Marquee.hlsl", L"PS", L"ps_6_0");
    
		// --- 2. ROOT SIGNATURE ---
		// En lugar de InitAsConstants, usamos InitAsConstantBufferView.
		// Esto encaja con context.SetConstantBuffer(0, alloc.GPUAddress) que usamos en el render.
		CD3DX12_ROOT_PARAMETER rootParams[1];
		// Visible en ALL porque el VS necesita la posición (RectMin/Max) y el PS necesita los colores
		rootParams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);

		m_marqueeRootSig = std::make_unique<RootSignature>(*m_device);
		m_marqueeRootSig->Initialize(1, rootParams);

		// --- 3. PIPELINE STATE OBJECT (PSO) ---
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    
		// CRÍTICO: El input layout queda vacío. No hay Vertex Buffer.
		psoDesc.InputLayout = { nullptr, 0 };
    
		psoDesc.pRootSignature = m_marqueeRootSig->GetNative();
		psoDesc.VS = { reinterpret_cast<BYTE*>(vertexShaderByteCode->GetBufferPointer()), vertexShaderByteCode->GetBufferSize() };
		psoDesc.PS = { reinterpret_cast<BYTE*>(pixelShaderByteCode->GetBufferPointer()), pixelShaderByteCode->GetBufferSize() };

		// Rasterizer: Sin Culling (Dibujamos un quad bidimensional)
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; 

		// Profundidad: Totalmente apagada para elementos de UI/Marquee
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

		// Blending: Alpha Blending tradicional
		D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
		blendDesc.BlendEnable = TRUE;
		blendDesc.LogicOpEnable = FALSE;
		blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; 
		blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; 
		blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.BlendState.RenderTarget[0] = blendDesc;
    
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    
		// OJO: Aunque la profundidad esté apagada, debes indicarle al PSO cuál es 
		// el formato de tu DSV actual, porque estará atado a la salida del pase de render.
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; 
		psoDesc.SampleDesc.Count = 1;

		// 4. Instanciar PSO
		m_marqueePSO = std::make_unique<GraphicsPipelineState>(*m_device);
		m_marqueePSO->Initialize(psoDesc);
	}

	void ScenePanel::SetCameraGizmoViewport()
	{
		auto mainViewport = m_sceneDocument->GetCamera().GetViewport();
		float twenty = m_form->Handle()->ToScale(20.0f);
		float gizmoCameraSize = m_form->Handle()->ToScale(Gizmo::CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS);
		Math::Viewport gizmoCameraViewport={
			mainViewport.width - gizmoCameraSize - twenty, 
			twenty, 
			gizmoCameraSize, 
			gizmoCameraSize
		};
		m_cameraGizmo->SetCameraGizmoViewport(gizmoCameraViewport);
	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer, m_sceneDocument->GetCamera());
	}

	void ScenePanel::RenderMarquee(GraphicsContext& context, const Math::Vector2& ndcMin, const Math::Vector2& ndcMax)
	{
		// 1. Setear Pipeline
		context.SetPipelineState(m_marqueePSO->GetNative());
		context.SetRootSignature(m_marqueeRootSig->GetNative());
		context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// 2. Preparar datos
		MarqueeData data;
		data.RectMin = ndcMin;
		data.RectMax = ndcMax;
		data.FillColor = Math::Color(0.2f, 0.5f, 1.0f, 0.3f);
		data.BorderColor = Math::Color(0.2f, 0.5f, 1.0f, 1.0f);
		data.BorderThicknessX = 2.0f / m_viewport.width;
		data.BorderThicknessY = 2.0f / m_viewport.height;

		// 3. Asignar memoria dinámica mágicamente alineada y lista
		DynamicAllocation alloc = context.AllocateDynamicSpace(sizeof(MarqueeData));
    
		// Copiar a la memoria persistente (CPU -> Upload Heap)
		memcpy(alloc.CPUAddress, &data, sizeof(MarqueeData));

		// 4. Bindear y Dibujar
		context.SetConstantBuffer(0, alloc.GPUAddress);
		context.DrawInstanced(4, 1, 0, 0);
	}

}
