#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace Bruno
{
	class GraphicsAdapter;
	using GraphicsAdapterList = std::vector<std::shared_ptr<GraphicsAdapter>>;

	class GraphicsAdapter
	{
	public:
		GraphicsAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter);
		~GraphicsAdapter() = default;

		static GraphicsAdapterList GetAdapters(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE);
		static std::shared_ptr<GraphicsAdapter> Create(DXGI_GPU_PREFERENCE gpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, bool useWarp = false);

		IDXGIAdapter4* GetHandle();
	private:
		std::string								m_devibceName;
		Microsoft::WRL::ComPtr<IDXGIAdapter4>	m_dxgiAdapter;
		DXGI_ADAPTER_DESC3						m_adapterDesc;
	};
}