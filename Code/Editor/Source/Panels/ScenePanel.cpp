#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/ResourceBarrier.h"

#include <Bruno/Platform/DirectX/IndexBuffer.h>
#include <Bruno/Platform/DirectX/VertexBuffer.h>
#include <Bruno/Platform/DirectX/Shader.h>

#include <nana/gui.hpp>
#include <iostream>
#include <Bruno/Core/Log.h>

namespace Bruno
{
	using namespace DirectX;

	// Vertex data for a colored cube.
	struct VertexPosColor
	{
		XMFLOAT3 Position;
		XMFLOAT3 Color;
	};

	static VertexPosColor g_Vertices[8] = {
	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },  // 0
	{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },   // 1
	{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) },    // 2
	{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },   // 3
	{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },   // 4
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) },    // 5
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) },     // 6
	{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }     // 7
	};

	static uint16_t g_Indices[36] = { 0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
							   3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7 };

	ScenePanel::ScenePanel(nana::window window) :
		nana::nested_form(window, nana::appear::bald<>())
	{
		this->caption("Scene");
		this->bgcolor(nana::colors::dark_red);

		static int idx = 0;
		idx++;
		idxx = idx;

		//TO-DO: ver si se puede agregar un evento al form o nested_form cuando llega un mensaje de WM_ACTIVATEAPP 
		//para luego disparar un evento y saber si el panel está activado o no. Es útil para el timer y el rendering/painting.
		this->events().unload([this](const nana::arg_unload& args)
		{
			BR_CORE_TRACE << "Unload or close panel id = " << idxx << std::endl;
			
		});
		this->events().expose([this](const nana::arg_expose& args) {

			BR_CORE_TRACE << "Expose panel id = " << idxx << ". exposed = " << args.exposed << std::endl;
			if (args.exposed) {
				m_timer.start();
			}
			else {
				m_timer.stop();
			}
			});
		this->events().resized([this](const nana::arg_resized& args) {
			BR_CORE_TRACE << "Resized panel id = " << idxx << ". " << args.width << "; " << args.height << std::endl;
			SurfaceWindowParameters parameters;
			parameters.Width = args.width;
			parameters.Height = args.height;
			parameters.WindowHandle = reinterpret_cast<HWND>(this->native_handle());

			if (m_surface != nullptr)
			{
				m_surface->Resize(args.width, args.height);
			}
			else
			{
				m_surface = std::make_unique<Surface>(parameters);
				m_surface->Initialize();
			}

			D3D12_CLEAR_VALUE optimizedClearValue = {};
			optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			optimizedClearValue.DepthStencil = { 1.0f, 0 };

			auto movil = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			auto movil2 = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, args.width, args.height,
				1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			ThrowIfFailed(Graphics::GetDevice()->GetD3DDevice()->CreateCommittedResource(
				&movil,
				D3D12_HEAP_FLAG_NONE,
				&movil2,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&optimizedClearValue,
				IID_PPV_ARGS(&m_depthBuffer)
			));

			// Update the depth-stencil view.
			D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
			dsv.Format = DXGI_FORMAT_D32_FLOAT;
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D.MipSlice = 0;
			dsv.Flags = D3D12_DSV_FLAG_NONE;

			Graphics::GetDevice()->GetD3DDevice()->CreateDepthStencilView(m_depthBuffer.Get(), &dsv,
				m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
		});


		m_indexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		m_vertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPosColor));

		m_vertexShader = std::make_unique<Shader>(L"VertexShader.hlsl", "main", "vs_5_1");
		m_pixelShader = std::make_unique<Shader>(L"PixelShader.hlsl", "main", "ps_5_1");

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		GraphicsDevice* device = Graphics::GetDevice();

		// Create the descriptor heap for the depth-stencil view.
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(device->GetD3DDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

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

		// Serialize the root signature.
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
			device->GetHighestRootSignatureVersion(), &rootSignatureBlob, &errorBlob));

		// Create the root signature.
		ThrowIfFailed(device->GetD3DDevice()->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

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
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = m_rootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader->GetBlob());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader->GetBlob());
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		ThrowIfFailed(device->GetD3DDevice()->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)));

		this->draw_through([this](){
			//BR_CORE_TRACE << "Paint panel. id = " << idxx << std::endl;
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
			auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

			m_commandList->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);
			m_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

			m_commandList->RSSetViewports(1, &m_surface->GetViewport());
			m_commandList->RSSetScissorRects(1, &m_surface->GetScissorRect());
			m_commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

			commandQueue->GetCommandList()->RSSetViewports(1, &m_surface->GetViewport());
			commandQueue->GetCommandList()->RSSetScissorRects(1, &m_surface->GetScissorRect());
			commandQueue->GetCommandList()->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);


			// Update the model matrix.
			
			float          angle = static_cast<float>(m_totalTime * 90.0);
			const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
			//XMMATRIX       modelMatrix = XMMatrixIdentity();
			XMMATRIX       modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
			m_totalTime += 0.016f;
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

			m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
			m_commandList->SetPipelineState(m_pipelineState.Get());
			m_commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);

			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetView());
			m_commandList->IASetIndexBuffer(&m_indexBuffer->GetView());
			m_commandList->DrawIndexedInstanced(m_indexBuffer->GetNumIndices(), 1, 0, 0, 0);

			ResourceBarrier::Transition(commandQueue->GetCommandList(),
				currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			commandQueue->EndFrame(m_surface.get());
		});
		
		this->show();

		HWND hwnd = reinterpret_cast<HWND>(this->native_handle());
		m_timer.elapse([hwnd, this] {
			BR_CORE_TRACE << "timer id = " << idxx << std::endl;
			RECT r;
			::GetClientRect(hwnd, &r);
			::InvalidateRect(hwnd, &r, FALSE);
		});

		m_timer.interval(std::chrono::milliseconds(16));
	}
}