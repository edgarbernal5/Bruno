#include "PlayerGame.h"

#include <Bruno/Platform/Windows/WindowsGameWindow.h>
#include <Bruno/Platform/DirectX/GraphicsAdapter.h>
#include <Bruno/Platform/DirectX/GraphicsDevice.h>
#include <Bruno/Platform/DirectX/Surface.h>
#include <Bruno/Platform/DirectX/CommandQueue.h>
#include <Bruno/Platform/DirectX/ResourceBarrier.h>
#include <Bruno/Platform/DirectX/VertexTypes.h>
#include <Bruno/Platform/DirectX/Texture.h>

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

	PlayerGame::PlayerGame(const Bruno::GameParameters& parameters)
		: Game(parameters)
	{
	}

	void PlayerGame::DoOnInitialize()
	{
		GameWindowParameters windowParameters;
		windowParameters.Width = m_parameters.WindowWidth;
		windowParameters.Height = m_parameters.WindowHeight;
		windowParameters.Title = m_parameters.Name;

		m_gameWindow = new WindowsGameWindow(windowParameters, this);
		m_gameWindow->Initialize();

		SurfaceWindowParameters surfaceParameters;
		surfaceParameters.Width= m_parameters.WindowWidth;
		surfaceParameters.Height = m_parameters.WindowHeight;
		surfaceParameters.WindowHandle = reinterpret_cast<HWND>(m_gameWindow->GetHandle());

		m_surface = std::make_unique<Surface>(surfaceParameters);
		m_surface->Initialize();

		m_indexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		m_vertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPositionColor));

		m_vertexShader = std::make_unique<Shader>(L"VertexShader.hlsl", "main", "vs_5_1");
		m_pixelShader = std::make_unique<Shader>(L"PixelShader.hlsl", "main", "ps_5_1");

		m_objectBuffer = std::make_unique<ConstantBuffer<ObjectBuffer>>();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_objectBuffer->GetResource()->GetGPUVirtualAddress();

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = 256;//d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

		m_device->GetD3DDevice()->CreateConstantBufferView(
			&cbvDesc,
			m_device->GetSrvDescriptionHeap().GetHeap()->GetCPUDescriptorHandleForHeapStart());

		//Texture* texture = new Texture(L"Textures/Mona_Lisa.jpg");
		GraphicsDevice* device = Graphics::GetDevice();

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		// A single 32-bit constant root parameter that is used by the vertex shader.
		CD3DX12_ROOT_PARAMETER rootParameters[1]{};
		//rootParameters[0].InitAsConstants(sizeof(XMMATRIX) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

		// Create a single descriptor table of CBVs.
		CD3DX12_DESCRIPTOR_RANGE cbvTable;
		cbvTable.Init(
			D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
			1, // Number of descriptors in table
			0); // base shader register arguments are bound to for this root parameter
		
		rootParameters[0].InitAsDescriptorTable(
				1, // Number of ranges
				&cbvTable); // Pointer to array of ranges

		//rootParameters[1].InitAsConstantBufferView(0);

		// A root signature is an array of root parameters.
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, rootParameters,
			0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		//m_rootSignature = std::make_unique<RootSignature>(rootSignatureDescription.Desc_1_1);

		// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			BR_CORE_ERROR << "Error during root signature creation. " << (char*)errorBlob->GetBufferPointer() << std::endl;
		}
		ThrowIfFailed(hr);

		ThrowIfFailed(m_device->GetD3DDevice()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(m_rootSignature.GetAddressOf())));
		
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

		pipelineStateStream.pRootSignature = m_rootSignature.Get();
		pipelineStateStream.InputLayout =  VertexPositionColor::InputLayout;
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader->GetBlob());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader->GetBlob());
		pipelineStateStream.DSVFormat = surfaceParameters.DepthBufferFormat;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		m_pipelineState = std::make_unique<PipelineStateObject>(pipelineStateStreamDesc);
		
		m_gameWindow->Show();
	}

	void PlayerGame::OnTick()
	{
		// Update the model matrix.
		static float TotalTime = 0.0f;
		float          angle = static_cast<float>(TotalTime * 90.0);
		const XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
		//XMMATRIX       modelMatrix = XMMatrixIdentity();
		XMMATRIX       modelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
		TotalTime += 0.0016f;
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
		
		ObjectBuffer objectBuffer;
		XMStoreFloat4x4(&objectBuffer.m_world, mvpMatrix);

		auto commandQueue = m_device->GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();
		commandQueue->BeginFrame();

		m_objectBuffer->CopyData(objectBuffer);

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

		ID3D12DescriptorHeap* descriptorHeaps[] = { m_device->GetSrvDescriptionHeap().GetHeap()};
		commandList->SetDescriptorHeaps(1, descriptorHeaps);

		commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		commandList->SetPipelineState(m_pipelineState->GetD3D12PipelineState());
		
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer->GetView());
		commandList->IASetIndexBuffer(&m_indexBuffer->GetView());

		//CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_device->GetSrvDescriptionHeap().GetHeap()->GetGPUDescriptorHandleForHeapStart());
		//commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / 4, &mvpMatrix, 0);


		//D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = m_objectBuffer->GetResource()->GetGPUVirtualAddress();
		//commandList->SetGraphicsRootConstantBufferView(0, objCBAddress);

		commandList->SetGraphicsRootDescriptorTable(0, m_device->GetSrvDescriptionHeap().GetHeap()->GetGPUDescriptorHandleForHeapStart());

		commandList->DrawIndexedInstanced(m_indexBuffer->GetNumIndices(), 1, 0, 0, 0);

		ResourceBarrier::Transition(commandQueue->GetCommandList(),
			currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		commandQueue->EndFrame(m_surface.get());
	}

	void PlayerGame::OnClientSizeChanged()
	{
		// Resize screen dependent resources.
		m_surface->Resize(m_gameWindow->GetWidth(), m_gameWindow->GetHeight());
	}
}