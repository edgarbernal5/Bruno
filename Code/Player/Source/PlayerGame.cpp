#include "PlayerGame.h"

#include <Bruno/Platform/Windows/WindowsGameWindow.h>
#include <Bruno/Platform/DirectX/GraphicsAdapter.h>
#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/CommandQueue.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Platform/DirectX/ShaderProgram.h>
#include <Bruno/Platform/DirectX/GraphicsContext.h>
#include <Bruno/Content/ContentManager.h>
#include <Bruno/Renderer/RenderItem.h>
#include <Bruno/Content/ContentTypeReaderManager.h>
#include <iostream>

namespace Bruno
{
	static VertexPositionNormalTexture g_Vertices[24] = {
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, 0.0f, -1.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(1.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(+1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(0.0f, 1.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 0.0f, 1.0f), Math::Vector2(1.0f, 0.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(0.0f, 1.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(1.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(0.0f, 1.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(0.0f, -1.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },

	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, 1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, 1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, 1.0f, -1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(-1.0f, -1.0f, -1.0f), Math::Vector3(-1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, -1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 1.0f)},
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, -1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(0.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, 1.0f, 1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 0.0f) },
	VertexPositionNormalTexture{ Math::Vector3(1.0f, -1.0f, 1.0f), Math::Vector3(1.0f, 0.0f, 0.0f), Math::Vector2(1.0f, 1.0f) },

	};

	static uint16_t g_Indices[36] = { 0, 1, 2,
		0, 2, 3,
		4, 5, 6, 
		4, 6, 7,
		8, 9, 10, 
		8, 10, 11,
		12, 13, 14, 
		12, 14, 15, 
		16, 17, 18, 
		16, 18, 19, 
		20, 21, 22,
		20, 22, 23 
	};

	PlayerGame::PlayerGame(const Bruno::GameParameters& parameters)
		: Game(parameters)
	{
	}

	void PlayerGame::OnInitialize(const GameWindowParameters& windowParameters)
	{
		m_gameWindow = std::make_unique<WindowsGameWindow>(windowParameters, this);
		m_gameWindow->Initialize();

		ContentTypeReaderManager::Initialize();

		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width = m_parameters.WindowWidth;
		surfaceParameters.Height = m_parameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_gameWindow->GetHandle());

		m_surface = std::make_unique<Surface>(surfaceParameters);
		m_surface->Initialize();

		auto boxRenderItem = std::make_shared<RenderItem>();
		boxRenderItem->IndexCount = (uint32_t)_countof(g_Indices);
		boxRenderItem->IndexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices) * sizeof(uint16_t), g_Indices, (uint32_t)sizeof(uint16_t));
		boxRenderItem->VertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices) * sizeof(VertexPositionNormalTexture), g_Vertices, (uint32_t)sizeof(VertexPositionNormalTexture));
		m_renderItems.push_back(boxRenderItem);

		m_opaqueShader = std::make_unique<Shader>(L"Opaque.hlsl");

		for (size_t i = 0; i < Graphics::Core::FRAMES_IN_FLIGHT_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}
		ContentManager manager(L"");
		m_texture = manager.Load<Texture>(L"Textures/Mona_Lisa.jpg");
		//m_texture = std::make_shared<Texture>(L"Textures/Mona_Lisa.jpg");
		//m_texture = std::make_unique<Texture>(L"Textures/Mona_Lisa.jpg");

		GraphicsDevice* device = Graphics::GetDevice();
		
		//m_rootSignature = m_opaqueShader->CreateRootSignature();

		PipelineResourceBinding textureBinding;
		textureBinding.BindingIndex = 0;
		textureBinding.Resource = m_texture.get();

		mMeshPerObjectResourceSpace.SetCBV(m_objectBuffer[0].get());
		mMeshPerObjectResourceSpace.SetSRV(textureBinding);
		mMeshPerObjectResourceSpace.Lock();

		PipelineResourceLayout meshResourceLayout;
		meshResourceLayout.Spaces[Graphics::Core::PER_OBJECT_SPACE] = &mMeshPerObjectResourceSpace;
		
		PipelineResourceMapping resourceMapping;

		m_rootSignature = std::make_unique<RootSignature>(meshResourceLayout, resourceMapping);

		m_graphicsContext = std::make_unique<GraphicsContext>(*device);

		GraphicsPipelineDesc meshPipelineDesc = GetDefaultGraphicsPipelineDesc();
		meshPipelineDesc.VertexShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Vertex);
		meshPipelineDesc.PixelShader = m_opaqueShader->GetShaderProgram(Shader::ShaderProgramType::Pixel);
		meshPipelineDesc.RenderTargetDesc.DepthStencilFormat = surfaceParameters.DepthBufferFormat;
		meshPipelineDesc.RenderTargetDesc.RenderTargetsCount = 1;
		meshPipelineDesc.DepthStencilDesc.DepthEnable = true;
		meshPipelineDesc.RenderTargetDesc.RenderTargetFormats[0] = surfaceParameters.BackBufferFormat;
		//meshPipelineDesc.DepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		m_pipelineState = std::make_unique<PipelineStateObject>(meshPipelineDesc, m_rootSignature.get(), resourceMapping);

		m_camera.LookAt(Math::Vector3(0, 0, -10), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0.0f, 0.0f, m_surface->GetViewport().Width, m_surface->GetViewport().Height), 0.1f, 100.0f);
		
		m_gameWindow->Show();
	}

	void PlayerGame::OnResize()
	{
		// Resize screen dependent resources.
		m_surface->Resize(m_gameWindow->GetWidth(), m_gameWindow->GetHeight());
		
		m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)m_gameWindow->GetWidth(), (float)m_gameWindow->GetHeight()));
		m_camera.UpdateMatrices();
	}
	
	void PlayerGame::OnGameLoop(const GameTimer& timer)
	{
		OnUpdate(m_timer);
		OnDraw();
	}

	void PlayerGame::OnUpdate(const GameTimer& timer)
	{
		//BR_CORE_TRACE << "delta time = " << timer.GetDeltaTime() << ". TotalTime " << TotalTime << std::endl;
		m_device->BeginFrame();

		UpdateCBs(timer);
	}

	void PlayerGame::OnDraw()
	{
		Math::Color clearColor { 1.0f, 1.0f, 0.0f, 1.0f };

		Texture& backBuffer = m_surface->GetBackBuffer();
		DepthBuffer& depthBuffer = m_surface->GetDepthBuffer();

		m_graphicsContext->Reset();
		m_graphicsContext->AddBarrier(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_graphicsContext->FlushBarriers();

		m_graphicsContext->ClearRenderTarget(backBuffer, clearColor);
		m_graphicsContext->ClearDepthStencilTarget(depthBuffer, 1.0f, 0);

		m_graphicsContext->SetViewport(m_surface->GetViewport());
		m_graphicsContext->SetScissorRect(m_surface->GetScissorRect());

		if (m_texture->IsReady()) {
			for (auto& item : m_renderItems)
			{
				if (!item->IndexBuffer->IsReady() || !item->VertexBuffer->IsReady())
					continue;

				m_graphicsContext->SetVertexBuffer(*item->VertexBuffer);
				m_graphicsContext->SetIndexBuffer(*item->IndexBuffer);

				PipelineResourceBinding textureBinding;
				textureBinding.BindingIndex = 0;
				textureBinding.Resource = m_texture.get();

				mMeshPerObjectResourceSpace.SetCBV(m_objectBuffer[m_device->GetFrameId()].get());
				mMeshPerObjectResourceSpace.SetSRV(textureBinding);

				PipelineInfo pipeline;
				pipeline.Pipeline = m_pipelineState.get();
				pipeline.RenderTargets.push_back(&backBuffer);
				pipeline.DepthStencilTarget = &depthBuffer;

				m_graphicsContext->SetPipeline(pipeline);
				m_graphicsContext->SetPipelineResources(Graphics::Core::PER_OBJECT_SPACE, mMeshPerObjectResourceSpace);

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

		m_device->SubmitContextWork(*m_graphicsContext);

		m_device->EndFrame();
		m_device->Present(m_surface.get());
	}

	void PlayerGame::OnMouseDown(MouseButtonState btnState, int x, int y)
	{
		m_lastMousePosition = Math::Int2(x, y);

		SetCapture(m_gameWindow->GetHandle());
	}

	void PlayerGame::OnMouseMove(MouseButtonState btnState, int x, int y)
	{
		Math::Int2 currentPosition = Math::Int2(x, y);
		if (btnState.LeftButton)
		{
			m_camera.Rotate(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		else if (btnState.MiddleButton)
		{
			m_camera.HandTool(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		else if (btnState.RightButton)
		{
			m_camera.PitchYaw(currentPosition, m_lastMousePosition);
			m_camera.UpdateMatrices();
		}
		m_lastMousePosition = currentPosition;
	}

	void PlayerGame::OnMouseUp(MouseButtonState btnState, int x, int y)
	{
		ReleaseCapture();
	}

	void PlayerGame::OnMouseWheel(MouseButtonState btnState, int x, int y, int wheelDelta) 
	{
		float zoom = wheelDelta * 0.0025f;
		m_camera.Zoom(zoom);
		m_camera.UpdateMatrices();
	}

	void PlayerGame::OnKeyPressed(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key pressed: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;
		if (key == KeyCode::A)
		{
			m_camera.Strafe(-0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::D)
		{
			m_camera.Strafe(0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::W)
		{
			m_camera.Walk(0.25f);
			m_camera.UpdateMatrices();
		}
		else if (key == KeyCode::S)
		{
			m_camera.Walk(-0.25f);
			m_camera.UpdateMatrices();
		}
	}

	void PlayerGame::OnKeyReleased(KeyCode key, KeyboardState state)
	{
		//BR_CORE_TRACE << "key released: " << (uint8_t)key << ". alt " << state.Alt << ". ctrl " << state.Ctrl << ". shift " << state.Shift << std::endl;
	}

	void PlayerGame::UpdateCBs(const GameTimer& timer)
	{
		static float TotalTime = 0.0f;
		float angle = static_cast<float>(0.0);
		//float angle = static_cast<float>(TotalTime * 45.0);
		
		Math::Matrix modelMatrix = Math::Matrix::Identity;
		//Math::Matrix modelMatrix = Math::Matrix::CreateFromAxisAngle(Math::Vector3(0, 1, 1), Math::ConvertToRadians(angle));
		TotalTime += timer.GetDeltaTime();

		Math::Matrix mvpMatrix = modelMatrix * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		uint32_t frameIndex = m_device->GetFrameId();
		m_objectBuffer[frameIndex]->SetMappedData(objectBuffer);
	}
}