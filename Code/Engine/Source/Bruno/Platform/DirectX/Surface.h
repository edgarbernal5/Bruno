#pragma once

#include <dxgi1_6.h>
#include "Resources.h"
#include "DepthBuffer.h"
#include <wrl/client.h>

namespace Bruno
{
	struct SurfaceWindowParameters
	{
		uint32_t Width;
		uint32_t Height;
		DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		uint32_t BackBufferCount = Graphics::Core::BACK_BUFFER_COUNT;
		DXGI_FORMAT DepthBufferFormat = DXGI_FORMAT_D32_FLOAT;
		HWND WindowHandle;
		bool Fullscreen = false;
	};

	class GraphicsDevice;
	class Texture;
	class DepthBuffer;

	class Surface
	{
	public:
		Surface(const SurfaceWindowParameters& parameters);
		~Surface();

		void Initialize();
		void Present() const;
		void Resize(uint32_t width, uint32_t height);

		Texture& GetBackBuffer() const { return *m_renderTargetData[m_currentBackBufferIndex].Resource; }
		constexpr const D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle() const { return m_renderTargetData[m_currentBackBufferIndex].RtvHandle.Cpu; }
		constexpr const D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle() const { return m_depthBuffer->GetDsvHandle().Cpu; }
		DepthBuffer& GetDepthBuffer() const { return *m_depthBuffer; }
		constexpr const D3D12_VIEWPORT& GetViewport() const { return m_viewport; }
		constexpr const D3D12_RECT& GetScissorRect() const { return m_scissorRect; }

		DXGI_FORMAT GetSurfaceFormat() const { return m_parameters.BackBufferFormat; }
		DXGI_FORMAT GetDepthBufferFormat() const { return m_parameters.DepthBufferFormat; }
	private:
		struct RenderTargetData
		{
			std::shared_ptr<Texture> Resource{ nullptr };
			DescriptorHandle RtvHandle{};
		};

		void HandleDeviceLost();
		void Finalize();
		void ReleaseBackBufferResources();

		SurfaceWindowParameters					m_parameters;

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapChain;
		RenderTargetData						m_renderTargetData[Graphics::Core::MAX_BACK_BUFFER_COUNT]{};
		mutable uint32_t						m_currentBackBufferIndex{ 0 };

		// Depth buffer.
		std::unique_ptr<DepthBuffer>			m_depthBuffer;

		D3D12_VIEWPORT							m_viewport;
		D3D12_RECT								m_scissorRect{};
	};
}