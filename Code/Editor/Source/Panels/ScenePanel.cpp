#include "ScenePanel.h"

#include "Bruno/Platform/DirectX/Surface.h"
#include "Bruno/Platform/DirectX/GraphicsDevice.h"
#include "Bruno/Platform/DirectX/ResourceBarrier.h"

#include <nana/gui.hpp>
#include <iostream>

namespace Bruno
{
	ScenePanel::ScenePanel(nana::window window) :
		nana::nested_form(window, nana::appear::bald<>())
	{
		this->caption("Scene");
		this->bgcolor(nana::colors::dark_red);

		static int idx = 0;
		idx++;
		idxx = idx;

		this->events().resized([this](const nana::arg_resized& args) {
			std::cout << "Resized panel id = " << idxx << ". " << args.width << "; " << args.height << std::endl;
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
		});

		this->draw_through([this](){
			std::cout << "Paint panel. id = " << idxx << std::endl;
			auto device = Bruno::Graphics::GetDevice();
			auto commandQueue = device->GetCommandQueue();
			commandQueue->BeginFrame();

			ID3D12Resource* const currentBackBuffer{ m_surface->GetBackBuffer() };
			ResourceBarrier::Transition(commandQueue->GetCommandList(),
				currentBackBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

			float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f }; //Yellow
			if (idxx == 2)clearColor[0] = 0.0f;

			commandQueue->GetCommandList()->RSSetViewports(1, &m_surface->GetViewport());
			commandQueue->GetCommandList()->ClearRenderTargetView(m_surface->GetRtv(), clearColor, 0, nullptr);

			ResourceBarrier::Transition(commandQueue->GetCommandList(),
				currentBackBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

			commandQueue->EndFrame(m_surface.get());
		});
		this->show();
	}
}