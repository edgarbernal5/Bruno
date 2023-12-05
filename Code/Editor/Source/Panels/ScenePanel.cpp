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
#include "EditorGame.h"

#include <nana/gui.hpp>
#include <iostream>
#include <Bruno/Core/Log.h>

namespace Bruno
{
	ScenePanel::ScenePanel(nana::window window, EditorGame *editorGame, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
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

			/*if (args.exposed) 
				this->focus();*/
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
			}
			m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)args.width, (float)args.height));
			m_isResizing = false;
		});

		m_form->events().mouse_down([this](const nana::arg_mouse& args)
		{
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;

			m_form->set_capture(true);
		});

		m_form->events().mouse_move([this](const nana::arg_mouse& args)
		{
			Math::Int2 currentPosition{ args.pos.x, args.pos.y };
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
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;
		});

		m_form->events().mouse_up([this](const nana::arg_mouse& args)
		{
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
		});

		InitializeMeshAndTexture();
		InitializeShaderAndPipeline();
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
	}

	void ScenePanel::OnDraw()
	{
		if (!m_isExposed || m_isResizing || m_isSizingMoving || !m_surface)
			return;

		auto device = Bruno::Graphics::GetDevice();
		Math::Color clearColor{ 1.0f, 1.0f, 0.0f, 1.0f };
		if (idxx == 2) {
			clearColor.R(0.0f);
		}else if (idxx == 3) {
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

		for (auto& item : m_renderItems)
		{
			auto texture = item->Material->TexturesByName["Texture"];
			if (texture != nullptr && texture->IsReady()) {
				if (!item->IndexBuffer->IsReady() || !item->VertexBuffer->IsReady())
					continue;

				m_graphicsContext->SetVertexBuffer(*item->VertexBuffer);
				m_graphicsContext->SetIndexBuffer(*item->IndexBuffer);

				PipelineResourceBinding textureBinding;
				textureBinding.BindingIndex = 0;
				textureBinding.Resource = texture.get();

				m_meshPerObjectResourceSpace.SetCBV(m_objectBuffer[device->GetFrameId()].get());
				m_meshPerObjectResourceSpace.SetSRV(textureBinding);

				PipelineInfo pipeline;
				pipeline.Pipeline = m_pipelineState.get();
				pipeline.RenderTargets.push_back(&backBuffer);
				pipeline.DepthStencilTarget = &depthBuffer;

				m_graphicsContext->SetPipeline(pipeline);
				m_graphicsContext->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, m_meshPerObjectResourceSpace);

				m_graphicsContext->SetPrimitiveTopology(item->PrimitiveType);
				m_graphicsContext->DrawIndexedInstanced(item->IndexCount,
					1,
					item->StartIndexLocation,
					item->BaseVertexLocation,
					0);
			}
		}

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
		m_camera.LookAt(Math::Vector3(0, 0, -10), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0, 0, 1, 1), 0.1f, 100.0f);
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

		auto& meshes = m_model->GetMeshes();
		for (auto& mesh : meshes)
		{
			auto boxRenderItem = std::make_shared<RenderItem>();
			boxRenderItem->IndexCount = mesh->GetIndexBuffer()->GetElementCount();
			boxRenderItem->IndexBuffer = mesh->GetIndexBuffer();
			boxRenderItem->VertexBuffer = mesh->GetVertexBuffer();
			boxRenderItem->Material = mesh->GetMaterial();
			m_renderItems.push_back(boxRenderItem);
		}

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}
	}

	void ScenePanel::InitializeShaderAndPipeline()
	{
		m_opaqueShader = std::make_unique<Shader>(L"Shaders/Opaque.hlsl");
		PipelineResourceBinding textureBinding;
		textureBinding.BindingIndex = 0;

		m_meshPerObjectResourceSpace.SetCBV(m_objectBuffer[0].get());
		m_meshPerObjectResourceSpace.SetSRV(textureBinding);
		m_meshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &m_meshPerObjectResourceSpace;

		PipelineResourceMapping resourceMapping;

		m_rootSignature = std::make_unique<RootSignature>(meshResourceLayout, resourceMapping);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = m_depthBufferFormat;
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = m_backBufferFormat;
		meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineState = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);

	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		auto device = Bruno::Graphics::GetDevice();
		static float TotalTime = 0.0f;
		float angle = static_cast<float>(0.0);
		//float angle = static_cast<float>(TotalTime * 45.0);

		Math::Matrix modelMatrix = Math::Matrix::Identity;
		//Math::Matrix modelMatrix = Math::Matrix::CreateFromAxisAngle(Math::Vector3(0, 1, 1), Math::ConvertToRadians(angle));
		TotalTime += timer.GetDeltaTime();

		Math::Matrix mvpMatrix = modelMatrix * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = device->GetFrameId();
		m_objectBuffer[frameIndex]->SetMappedData(objectBuffer);
	}
}