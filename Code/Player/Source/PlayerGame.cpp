#include "PlayerGame.h"

#include <Bruno/Platform/Windows/WindowsGameWindow.h>
#include <Bruno/Platform/DirectX/GraphicsAdapter.h>
#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/CommandQueue.h>
#include <Bruno/Platform/DirectX/ResourceBarrier.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Renderer/RenderItem.h>
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

		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width = m_parameters.WindowWidth;
		surfaceParameters.Height = m_parameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_gameWindow->GetHandle());

		m_surface = std::make_unique<Surface>(surfaceParameters);
		m_surface->Initialize();

		auto boxRenderItem = std::make_shared<RenderItem>();
		boxRenderItem->IndexCount = (uint32_t)_countof(g_Indices);
		boxRenderItem->IndexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		boxRenderItem->VertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPositionNormalTexture));
		m_renderItems.push_back(boxRenderItem);

		//m_vertexShader = std::make_unique<ShaderProgram>(L"VertexShader.hlsl", "main", "vs_5_1");
		//m_pixelShader = std::make_unique<ShaderProgram>(L"PixelShader.hlsl", "main", "ps_5_1");

		for (size_t i = 0; i < Graphics::Core::FRAME_BUFFER_COUNT; i++)
		{
			m_objectBuffer[i] = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		}
		m_texture = std::make_unique<Texture>(L"Textures/Mona_Lisa.jpg");

		GraphicsDevice* device = Graphics::GetDevice();

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_DESCRIPTOR_RANGE texTable;
		texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER rootParameters[2]{};
		// Perfomance TIP: Order from most frequent to least frequent.
		rootParameters[0].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsConstantBufferView(0);

		CD3DX12_STATIC_SAMPLER_DESC linearRepeatSampler(0, D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR);
		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription(2, rootParameters,
			1, &linearRepeatSampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		m_rootSignature = std::make_unique<RootSignature>(rootSignatureDescription);

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = surfaceParameters.BackBufferFormat;

		pipelineStateStream.pRootSignature = m_rootSignature->GetD3D12RootSignature();
		pipelineStateStream.InputLayout = VertexPositionNormalTexture::InputLayout;
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		//pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader->GetBlob());
		//pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader->GetBlob());
		pipelineStateStream.DSVFormat = surfaceParameters.DepthBufferFormat;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		m_pipelineState = std::make_unique<PipelineStateObject>(pipelineStateStreamDesc);
		
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
		
		auto commandQueue = m_device->GetCommandQueue();
		commandQueue->WaitFrame();

		UpdateCBs(timer);
	}

	void PlayerGame::OnDraw()
	{
		auto commandQueue = m_device->GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();
		int frameIndex = commandQueue->GetFrameIndex();

		commandQueue->BeginFrame();
		ID3D12Resource* const currentBackBuffer{ m_surface->GetBackBuffer() };
		ResourceBarrier::Transition(commandList,
			currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };

		auto rtv = m_surface->GetRtv();
		auto dsv = m_surface->GetDsv();

		commandList->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		commandList->RSSetViewports(1, &m_surface->GetViewport());
		commandList->RSSetScissorRects(1, &m_surface->GetScissorRect());
		commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_device->GetSrvDescriptionHeap().GetHeap() };
		commandList->SetDescriptorHeaps(1, descriptorHeaps);

		commandList->SetGraphicsRootSignature(m_rootSignature->GetD3D12RootSignature());
		commandList->SetPipelineState(m_pipelineState->GetD3D12PipelineState());

		for (auto& item : m_renderItems)
		{
			commandList->IASetPrimitiveTopology(item->PrimitiveType);
			commandList->IASetVertexBuffers(0, 1, &item->VertexBuffer->GetView());
			commandList->IASetIndexBuffer(&item->IndexBuffer->GetView());

			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_texture->GetSrvHandle().Gpu);

			D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_objectBuffer[frameIndex]->GetResource()->GetGPUVirtualAddress();

			commandList->SetGraphicsRootDescriptorTable(0, tex);
			commandList->SetGraphicsRootConstantBufferView(1, objCBAddress);

			commandList->DrawIndexedInstanced(item->IndexCount,
				1,
				item->StartIndexLocation,
				item->BaseVertexLocation,
				0);
		}

		ResourceBarrier::Transition(commandQueue->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		commandQueue->EndFrame(m_surface.get());
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
		auto commandQueue = m_device->GetCommandQueue();

		static float TotalTime = 0.0f;
		float angle = static_cast<float>(0.0);
		//float angle = static_cast<float>(TotalTime * 45.0);
		
		Math::Matrix modelMatrix = Math::Matrix::Identity;
		//Math::Matrix modelMatrix = Math::Matrix::CreateFromAxisAngle(Math::Vector3(0, 1, 1), Math::ConvertToRadians(angle));
		TotalTime += timer.GetDeltaTime();

		Math::Matrix mvpMatrix = modelMatrix * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		int frameIndex = commandQueue->GetFrameIndex();
		m_objectBuffer[frameIndex]->CopyData(objectBuffer);
	}
}