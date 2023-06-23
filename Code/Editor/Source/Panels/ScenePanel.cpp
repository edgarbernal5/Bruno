#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/ResourceBarrier.h"

#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include "EditorGame.h"

#include <nana/gui.hpp>
#include <iostream>
#include <Bruno/Core/Log.h>

namespace Bruno
{
	using namespace DirectX;

	static VertexPositionColor g_Vertices[8] = {
	VertexPositionColor{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },  // 0
	VertexPositionColor{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },   // 1
	VertexPositionColor{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) },    // 2
	VertexPositionColor{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },   // 3
	VertexPositionColor{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },   // 4
	VertexPositionColor{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },    // 5
	VertexPositionColor{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },     // 6
	VertexPositionColor{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }     // 7
	};

	static uint16_t g_Indices[36] = { 0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
							   3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7 };

	ScenePanel::ScenePanel(nana::window window, EditorGame *editorGame, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
		nana::nested_form(window, nana::appear::bald<>()),
		m_backBufferFormat(backBufferFormat),
		m_depthBufferFormat(depthBufferFormat)
	{
		this->caption("Scene");
		this->bgcolor(nana::colors::dark_red);

		static int idx = 0;
		idx++;
		idxx = idx;

		//TO-DO: ver si se puede agregar un evento al form o nested_form cuando llega un mensaje de WM_ACTIVATEAPP 
		//para luego disparar un evento y saber si el panel est� activado o no. Es �til para el timer y el rendering/painting.
		/*
		1. quitar el mensaje WM_ACTIVATEAPP como trivial (bedrock_windows.cpp)
		2. crear el evento (en general_events.hpp)
		3. crear el event code (event_code.hpp)
		4. ver y analizar metodo bedrock::event_expose para emitir el evento
		*/
		this->events().unload([editorGame, this](const nana::arg_unload& args)
		{
			BR_CORE_TRACE << "Unload or close panel id = " << idxx << std::endl;
			auto device = Graphics::GetDevice();
			device->GetCommandQueue()->Flush();

			editorGame->RemoveScenePanel(this);
		});
		
		this->events().expose([this](const nana::arg_expose& args)
		{
			BR_CORE_TRACE << "Expose panel id = " << idxx << ". exposed = " << args.exposed << std::endl;
			m_isExposed = args.exposed;
		});

		this->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			BR_CORE_TRACE << "enter_size_move /panel id = " << idxx << std::endl;
		});
		this->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			BR_CORE_TRACE << "exit_size_move /panel id = " << idxx << std::endl;
		});

		this->events().activate([this](const nana::arg_activate& args)
		{
			BR_CORE_TRACE << "activate /panel id = " << idxx << std::endl;
		});
		this->events().resized([this](const nana::arg_resized& args) {
			//BR_CORE_TRACE << "Resized panel id = " << idxx << ". hwnd = " << this->native_handle() << "." << args.width << "; " << args.height << std::endl;
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
				parameters.WindowHandle = reinterpret_cast<HWND>(this->native_handle());

				m_surface = std::make_unique<Surface>(parameters);
				m_surface->Initialize();
			}
			m_isResizing = false;
		});

		m_indexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		m_vertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPositionColor));

		m_vertexShader = std::make_unique<Shader>(L"VertexShader.hlsl", "main", "vs_5_1");
		m_pixelShader = std::make_unique<Shader>(L"PixelShader.hlsl", "main", "ps_5_1");

		GraphicsDevice* device = Graphics::GetDevice();

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER1 rootParameters[1]{};
		rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;
		rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);
		
		m_rootSignature = std::make_unique<RootSignature>(rootSignatureDescription.Desc_1_1);

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
		rtvFormats.RTFormats[0] = m_backBufferFormat;

		pipelineStateStream.pRootSignature = m_rootSignature->GetD3D12RootSignature();
		pipelineStateStream.InputLayout = VertexPositionColor::InputLayout;
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader->GetBlob());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader->GetBlob());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		m_pipelineState = std::make_unique<PipelineStateObject>(pipelineStateStreamDesc);

		editorGame->AddScenePanel(this);

		this->show();
	}

	ScenePanel::~ScenePanel()
	{
		BR_CORE_TRACE << "destructor panel id = " << idxx << std::endl;

	}
	
	void ScenePanel::OnUpdate(const GameTimer& timer)
	{
		if (!enabled() || !m_isExposed || m_isResizing)
			return;

		//BR_CORE_TRACE << "Paint panel. id = " << idxx << ". delta time = " << timer.GetDeltaTime() << std::endl;
		auto device = Bruno::Graphics::GetDevice();
		auto commandQueue = device->GetCommandQueue();
		auto m_commandList = commandQueue->GetCommandList();
		commandQueue->BeginFrame();

		ID3D12Resource* const currentBackBuffer{ m_surface->GetBackBuffer() };
		ResourceBarrier::Transition(commandQueue->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; //Yellow
		if (idxx == 2) clearColor[0] = 0.0f;

		auto rtv = m_surface->GetRtv();
		auto dsv = m_surface->GetDsv();

		m_commandList->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);
		m_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_commandList->RSSetViewports(1, &m_surface->GetViewport());
		m_commandList->RSSetScissorRects(1, &m_surface->GetScissorRect());
		m_commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		commandQueue->GetCommandList()->RSSetViewports(1, &m_surface->GetViewport());
		commandQueue->GetCommandList()->RSSetScissorRects(1, &m_surface->GetScissorRect());
		commandQueue->GetCommandList()->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);

		// Update the model matrix.

		float          angle = static_cast<float>(m_totalTime * 45.0f);
		const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
		//XMMATRIX       modelMatrix = XMMatrixIdentity();
		XMMATRIX       modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
		//BR_CORE_TRACE << " panelid = " << idxx << ". delta time = " << m_gameTimer.GetDeltaTime() << ". TotalTime " << m_totalTime << std::endl;
		m_totalTime += timer.GetDeltaTime();
		// Update the view matrix.
		const XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
		const XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
		const XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		XMMATRIX       viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

		// Update the projection matrix.
		float    aspectRatio = m_surface->GetViewport().Width / m_surface->GetViewport().Height;
		XMMATRIX projectionMatrix =
			XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspectRatio, 0.1f, 1000.0f);
		XMMATRIX mvpMatrix = XMMatrixMultiply(modelMatrix, viewMatrix);
		mvpMatrix = XMMatrixMultiply(mvpMatrix, projectionMatrix);

		m_commandList->SetGraphicsRootSignature(m_rootSignature->GetD3D12RootSignature());
		m_commandList->SetPipelineState(m_pipelineState->GetD3D12PipelineState());
		m_commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetView());
		m_commandList->IASetIndexBuffer(&m_indexBuffer->GetView());
		m_commandList->DrawIndexedInstanced(m_indexBuffer->GetNumIndices(), 1, 0, 0, 0);

		ResourceBarrier::Transition(commandQueue->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		commandQueue->EndFrame(m_surface.get());
	}

	void ScenePanel::OnDraw()
	{
	}
}