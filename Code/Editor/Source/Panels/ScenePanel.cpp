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

	ScenePanel::ScenePanel(nana::window window, EditorGame *editorGame, DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat) :
		nana::nested_form(window, nana::appear::bald<>()),
		m_backBufferFormat(backBufferFormat),
		m_depthBufferFormat(depthBufferFormat),

		m_editorGame(editorGame)
	{
		this->caption("Scene");
		this->bgcolor(nana::colors::dark_red);

		static int idx = 0;
		idx++;
		idxx = idx;

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
		
		this->events().expose([this](const nana::arg_expose& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "Expose panel id = " << idxx << ". exposed = " << args.exposed << std::endl;

			m_isExposed = args.exposed;
		});

		this->events().enter_size_move([this](const nana::arg_size_move& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "enter_size_move /panel id = " << idxx << std::endl;

			m_isSizingMoving = true;
		});

		this->events().exit_size_move([this](const nana::arg_size_move& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "exit_size_move /panel id = " << idxx << std::endl;

			if (m_surface)
			{
				m_surface->Resize(this->size().width, this->size().height);
			}
			m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)this->size().width, (float)this->size().height));
			m_camera.UpdateMatrices();
			m_isSizingMoving = false;
		});

		this->events().resized([this](const nana::arg_resized& args)
		{
			std::lock_guard lock{ m_mutex };
			BR_CORE_TRACE << "Resized panel id = " << idxx << ". hwnd = " << this->native_handle() << "." << args.width << "; " << args.height << std::endl;

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
				parameters.WindowHandle = reinterpret_cast<HWND>(this->native_handle());

				m_surface = std::make_unique<Surface>(parameters);
				m_surface->Initialize();
			}
			m_camera.SetViewport(Math::Viewport(0.0f, 0.0f, (float)args.width, (float)args.height));
			m_camera.UpdateMatrices();
			m_isResizing = false;
		});

		m_indexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		m_vertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPositionColor));

		m_vertexShader = std::make_unique<Shader>(L"VertexShader.hlsl", "main", "vs_5_1");
		m_pixelShader = std::make_unique<Shader>(L"PixelShader.hlsl", "main", "ps_5_1");

		auto boxRenderItem = std::make_shared<RenderItem>();
		boxRenderItem->IndexCount = (uint32_t)_countof(g_Indices);
		boxRenderItem->IndexBuffer = std::make_unique<IndexBuffer>((uint32_t)_countof(g_Indices), g_Indices, (uint32_t)sizeof(uint16_t));
		boxRenderItem->VertexBuffer = std::make_unique<VertexBuffer>((uint32_t)_countof(g_Vertices), g_Vertices, (uint32_t)sizeof(VertexPositionNormalTexture));
		m_renderItems.push_back(boxRenderItem);

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
		rtvFormats.RTFormats[0] = m_backBufferFormat;

		pipelineStateStream.pRootSignature = m_rootSignature->GetD3D12RootSignature();
		pipelineStateStream.InputLayout = VertexPositionNormalTexture::InputLayout;
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader->GetBlob());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader->GetBlob());
		pipelineStateStream.DSVFormat = m_depthBufferFormat;
		pipelineStateStream.RTVFormats = rtvFormats;

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		m_pipelineState = std::make_unique<PipelineStateObject>(pipelineStateStreamDesc);

		m_camera.LookAt(Math::Vector3(0, 0, -10), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));
		m_camera.SetLens(Math::ConvertToRadians(45.0f), Math::Viewport(0, 0, 1, 1), 0.1f, 100.0f);

		this->events().mouse_down([this](const nana::arg_mouse& args)
		{
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;
		});
		
		this->events().mouse_move([this](const nana::arg_mouse& args)
		{
			Math::Int2 currentPosition = Math::Int2(args.pos.x, args.pos.y);
			if (args.left_button)
			{
				m_camera.Rotate(currentPosition, m_lastMousePosition);
				m_camera.UpdateMatrices();
			}
			m_lastMousePosition.x = args.pos.x;
			m_lastMousePosition.y = args.pos.y;
		});

		this->events().mouse_wheel([this](const nana::arg_wheel& args)
		{
			BR_CORE_TRACE << "mouse wheel = " << args.distance << std::endl;
			float zoom = args.distance * 0.0025f;
			if (args.upwards) zoom = -zoom;

			m_camera.Zoom(zoom);
			m_camera.UpdateMatrices();
		});

		{
			std::lock_guard lock{ m_mutex };
			editorGame->AddScenePanel(this);
		}

		this->show();
	}

	ScenePanel::~ScenePanel()
	{
		std::lock_guard lock{ m_mutex };
		BR_CORE_TRACE << "destructor panel id = " << idxx << std::endl;

		auto device = Graphics::GetDevice();
		device->GetCommandQueue()->Flush();

		m_editorGame->RemoveScenePanel(this);
	}
	
	void ScenePanel::OnUpdate(const GameTimer& timer)
	{
		//BR_CORE_TRACE << "Paint panel. id = " << idxx << ". delta time = " << timer.GetDeltaTime() << std::endl;
		std::lock_guard lock{ m_mutex };

		if (!m_isExposed || m_isResizing || m_isSizingMoving)
			return;

		auto device = Bruno::Graphics::GetDevice();
		auto commandQueue = device->GetCommandQueue();
		commandQueue->WaitFrame();

		UpdateCBs(timer);
	}

	void ScenePanel::OnDraw()
	{
		std::lock_guard lock{ m_mutex };

		if (!m_isExposed || m_isResizing || m_isSizingMoving)
			return;

		auto device = Bruno::Graphics::GetDevice();
		auto commandQueue = device->GetCommandQueue();
		auto commandList = commandQueue->GetCommandList();
		int frameIndex = commandQueue->GetFrameIndex();

		commandQueue->BeginFrame();
		ID3D12Resource* const currentBackBuffer{ m_surface->GetBackBuffer() };
		ResourceBarrier::Transition(commandList,
			currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

		float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; //Yellow
		if (idxx == 2) clearColor[0] = 0.0f;

		auto rtv = m_surface->GetRtv();
		auto dsv = m_surface->GetDsv();

		commandList->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		commandList->RSSetViewports(1, &m_surface->GetViewport());
		commandList->RSSetScissorRects(1, &m_surface->GetScissorRect());
		commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		ID3D12DescriptorHeap* descriptorHeaps[] = { device->GetSrvDescriptionHeap().GetHeap() };
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

		ResourceBarrier::Transition(commandList,
			currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		commandQueue->EndFrame(m_surface.get());
	}

	bool ScenePanel::IsEnabled()
	{
		std::lock_guard lock{ m_mutex };

		return (m_isExposed && !m_isResizing && !m_isSizingMoving);
	}

	void ScenePanel::UpdateCBs(const GameTimer& timer)
	{
		auto device = Bruno::Graphics::GetDevice();
		auto commandQueue = device->GetCommandQueue();

		// Update the model matrix.
		//float angle = static_cast<float>(m_totalTime * 45.0);
		float angle = static_cast<float>(0.0);

		Math::Matrix modelMatrix = Math::Matrix::CreateFromAxisAngle(Math::Vector3(0, 1, 1), Math::ConvertToRadians(angle));
		//Math::Matrix modelMatrix = Math::Matrix::Identity;
		m_totalTime += timer.GetDeltaTime();

		Math::Matrix mvpMatrix = modelMatrix * m_camera.GetViewProjection();

		ObjectBuffer objectBuffer;
		objectBuffer.World = mvpMatrix;

		int frameIndex = commandQueue->GetFrameIndex();
		m_objectBuffer[frameIndex]->CopyData(objectBuffer);
	}
}