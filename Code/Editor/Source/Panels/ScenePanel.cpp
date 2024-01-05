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
#include <Bruno/Editor/ObjectSelector.h>
#include "EditorGame.h"

#include <nana/gui.hpp>
#include <iostream>
#include <Bruno/Core/Log.h>

namespace Bruno
{
	ScenePanel::ScenePanel(nana::window window, EditorGame* editorGame, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
		nana::nested_form(window, nana::appear::bald<>()),
		//nana::panel<true>(window),
		m_backBufferFormat(backBufferFormat),
		m_depthBufferFormat(depthBufferFormat),

		m_editorGame(editorGame)
	{
		m_form = this;
		static int idx = 0;
		idx++;
		idxx = idx;

		std::ostringstream idstr;
		idstr << "Scene id " << idxx;
		this->caption(idstr.str());
		this->bgcolor(nana::colors::dark_red);

		//m_form = std::make_unique<nana::nested_form>(this->handle(), nana::appear::bald<>());

		//TO-DO: ver si se puede agregar un evento al form o nested_form cuando llega un mensaje de WM_ACTIVATEAPP 
		//para luego disparar un evento y saber si el panel está activado o no. Es útil para el timer y el rendering/painting.
		/*
		1. quitar el mensaje WM_ACTIVATEAPP como trivial (bedrock_windows.cpp)
		2. crear el evento (en general_events.hpp)
		3. crear el event code (event_code.hpp)
		4. ver y analizar metodo bedrock::event_expose para emitir el evento
		*/

		// Single-thread rendering.
		//auto hwnd = reinterpret_cast<HWND>(this->native_handle());
		//this->draw_through([editorGame, hwnd, this]
		//{
		//	editorGame->OnTick();

		//	/*RECT r;
		//	::GetClientRect(hwnd, &r);
		//	::InvalidateRect(hwnd, &r, FALSE);*/
		//});

		this->events().destroy([this](const nana::arg_destroy& args) {
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "destroy. panel id = " << idxx << std::endl;

			auto device = Graphics::GetDevice();
			device->WaitForIdle();

			m_isExposed = false;
			m_editorGame->RemoveScenePanel(this);
		});

		//this->events().resized([this](const nana::arg_resized& args)
		//{
		//	nana::size newSize(args.width, args.height);
		//	m_form->size(newSize);
		//});

		this->events().expose([this](const nana::arg_expose& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "Expose panel id = " << idxx << ". exposed = " << args.exposed << std::endl;

			m_isExposed = args.exposed;
			if (m_isExposed)
				m_form->show();
			else
				m_form->hide();

			//if (args.exposed)
			//	this->focus();
		});

		//m_form->events().expose([this](const nana::arg_expose& args)
		//{
		//	std::lock_guard lock{ m_mutex };
		//	BR_CORE_TRACE << "Expose panel id = " << idxx << ". exposed = " << args.exposed << std::endl;

		//	m_isExposed = args.exposed;
		//	/*if (m_isExposed)
		//		m_form->show();
		//	else
		//		m_form->hide();*/

		//	/*if (args.exposed) 
		//		this->focus();*/
		//});

		m_form->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "enter_size_move /panel id = " << idxx << std::endl;

			m_isSizingMoving = true;
		});

		m_form->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "exit_size_move /panel id = " << idxx << std::endl;

			if (m_surface)
			{
				m_surface->Resize(this->size().width, this->size().height);
			}
			m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)this->size().width, (float)this->size().height));
			m_isSizingMoving = false;
		});

		m_form->events().resized([this](const nana::arg_resized& args)
		{
			std::lock_guard lock{ m_mutex };
			//BR_CORE_TRACE << "Resized panel id = " << idxx << ". hwnd = " << m_form->native_handle() << ". w=" << args.width << "; h=" << args.height << std::endl;

			if (m_isSizingMoving)
				return;

			m_isResizing = true;

			if (m_surface)
			{
				m_surface->Resize(args.width, args.height);
			}
			else
			{
				SurfaceWindowParameters parameters;
				parameters.Width = args.width;
				parameters.Height = args.height;
				parameters.BackBufferFormat = m_backBufferFormat;
				parameters.DepthBufferFormat = m_depthBufferFormat;
				parameters.WindowHandle = reinterpret_cast<HWND>(m_form->native_handle());

				m_surface = std::make_unique<Surface>(parameters);
				m_surface->Initialize();

				//TODO: esta inicialización está acá porque depende del surface. Esto está mal, arreglarlo!
				InitializeMeshAndTexture();
				InitializeGizmoService();
			}
			m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)args.width, (float)args.height));
			m_isResizing = false;
		});

		m_form->events().mouse_down([this](const nana::arg_mouse& args)
		{
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;

			m_isGizmoing = m_gizmoService->BeginDrag(Math::Vector2(args.pos.x, args.pos.y));
			m_form->set_capture(true);
		});

		m_form->events().mouse_move([this](const nana::arg_mouse& args)
		{
			Math::Int2 currentPosition{ args.pos.x, args.pos.y };

			if (!m_isGizmoing && !args.left_button) {
				m_gizmoService->OnMouseMove(Math::Vector2(args.pos.x, args.pos.y));
			}
			if (args.left_button && m_isGizmoing) {
				m_gizmoService->Drag(Math::Vector2(args.pos.x, args.pos.y));
			}
			else if (args.shift)
			{
				if (args.left_button)
				{
					m_camera.Rotate(currentPosition, m_lastMousePosition);
				}
				else if (args.mid_button)
				{
					m_camera.HandTool(currentPosition, m_lastMousePosition);
				}
				else if (args.right_button)
				{
					m_camera.PitchYaw(currentPosition, m_lastMousePosition);
				}
			}
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;
		});

		m_form->events().mouse_up([this](const nana::arg_mouse& args)
		{
			if (m_isGizmoing)
			{
				m_gizmoService->EndDrag();
				m_isGizmoing = false;
			}
			m_form->release_capture();
		});

		m_form->events().mouse_wheel([this](const nana::arg_wheel& args)
		{
			float zoom = args.distance * 0.0025f;
			if (!args.upwards) zoom = -zoom;

			m_camera.Zoom(zoom);
		});

		m_form->events().key_press([this](const nana::arg_keyboard& args)
		{
			if (args.key == 'A')
			{
				m_camera.Strafe(-0.25f);
			}
			else if (args.key == 'D')
			{
				m_camera.Strafe(0.25f);
			}
			else if (args.key == 'W')
			{
				m_camera.Walk(0.25f);
			}
			else if (args.key == 'S')
			{
				m_camera.Walk(-0.25f);
			}
			else if (args.key == '1')
			{
				m_gizmoService->SetGizmoType(GizmoService::GizmoType::Translation);
			}
			else if (args.key == '2')
			{
				m_gizmoService->SetGizmoType(GizmoService::GizmoType::Rotation);
			}
			else if (args.key == '3')
			{
				m_gizmoService->SetGizmoType(GizmoService::GizmoType::Scale);
			}
			else if (args.key == '4')
			{
				if (m_gizmoService->GetTransformSpace() == GizmoService::TransformSpace::World)
					m_gizmoService->SetTransformSpace(GizmoService::TransformSpace::Local);
				else
					m_gizmoService->SetTransformSpace(GizmoService::TransformSpace::World);
			}
		});

		InitializeCamera();
		InitializeGraphicsContext();

		editorGame->AddScenePanel(this);
		m_form->show();
		//m_isExposed = true;
	}

	ScenePanel::~ScenePanel()
	{
		std::lock_guard lock{ m_mutex };
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

		auto device = Bruno::Graphics::GetDevice();
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
			clearColor.R(0.0f);
		}
		else if (idxx == 3) {
			clearColor.R(0.0f);
			clearColor.G(0.0f);
		}
		else if (idxx == 4) {
			clearColor.R(0.0f);
			clearColor.G(0.0f);
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

		////test gizmo
		m_gizmoService->Render(m_graphicsContext.get());

		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_graphicsContext->FlushBarriers();

		device->SubmitContextWork(*m_graphicsContext);

		device->EndFrame();
		device->Present(m_surface.get());
	}

	bool ScenePanel::IsEnabled()
	{
		std::lock_guard lock{ m_mutex };

		return (m_isExposed && !m_isResizing && !m_isSizingMoving);
	}

	void ScenePanel::InitializeCamera()
	{
		m_camera.LookAt(Math::Vector3(0, 0, -25), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0, 0, 1, 1), 0.1f, 100.0f);
	}

	void ScenePanel::InitializeGizmoService()
	{
		auto device = Graphics::GetDevice();
		m_objectSelector = std::make_shared<ObjectSelector>(m_scene);

		m_gizmoService = std::make_unique<GizmoService>(device, m_camera, m_surface.get(), m_objectSelector.get());
		m_gizmoService->SetTranslationCallback([&](const Math::Vector3& delta)
		{
			m_objectSelector->GetSelectedObjects()[0]->Position += delta;
		});

		m_gizmoService->SetRotationCallback([&](const Math::Quaternion& delta)
		{
			auto newRotation = m_objectSelector->GetSelectedObjects()[0]->Rotation * delta;
			newRotation.Normalize();
			m_objectSelector->GetSelectedObjects()[0]->Rotation = newRotation;
		});

		m_gizmoService->SetScaleCallback([&](const Math::Vector3& delta, bool isUniform)
		{
			auto newDelta = delta * 0.1f;
			if (isUniform)
			{
				float uniformDelta = 1.0f + (newDelta.x + newDelta.y + newDelta.z) / 3.0f;
				auto newScale = m_objectSelector->GetSelectedObjects()[0]->Scale * uniformDelta;
				if (newScale.x > 0.001f && newScale.y > 0.001f && newScale.z > 0.001f)
					m_objectSelector->GetSelectedObjects()[0]->Scale = newScale;

				return;
			}
			auto newScale = m_objectSelector->GetSelectedObjects()[0]->Scale + newDelta;
			if (newScale.x > 0.001f && newScale.y > 0.001f && newScale.z > 0.001f)
				m_objectSelector->GetSelectedObjects()[0]->Scale = newScale;
		});
	}

	void ScenePanel::InitializeGraphicsContext()
	{
		GraphicsDevice* device = Graphics::GetDevice();
		m_graphicsContext = std::make_unique<GraphicsContext>(*device);
	}

	void ScenePanel::InitializeMeshAndTexture()
	{
		ContentManager manager(m_editorGame->m_applicationParameters.WorkingDirectory);

		m_model = manager.Load<Model>(L"Models\\Car\\Car.fbx");

		m_scene = std::make_shared<Scene>(m_camera);
		m_scene->InstantiateModel(m_model);
		m_sceneRenderer = std::make_shared<SceneRenderer>(m_scene, m_surface.get());
	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		m_scene->OnUpdate(timer);
	}
}
