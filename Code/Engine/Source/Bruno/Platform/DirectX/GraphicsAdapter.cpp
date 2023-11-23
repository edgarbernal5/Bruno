#include "brpch.h"
#include "GraphicsAdapter.h"

#include "D3DHelpers.h"

namespace Bruno
{
	GraphicsAdapterList GraphicsAdapter::GetAdapters(DXGI_GPU_PREFERENCE gpuPreference)
	{
		uint32_t createFactoryFlags = 0;
#ifdef BR_DEBUG
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		GraphicsAdapterList adapters;

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
		Microsoft::WRL::ComPtr<IDXGIAdapter>  dxgiAdapter;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

		ThrowIfFailed(::CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory6)));

		for (uint32_t adapterIndex = 0; dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
		{
			if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
			{
				ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
				std::shared_ptr<GraphicsAdapter> adapter = std::make_shared<GraphicsAdapter>(dxgiAdapter4);
				adapters.push_back(adapter);
			}
		}

		return adapters;
	}

	std::shared_ptr<GraphicsAdapter> GraphicsAdapter::Create(DXGI_GPU_PREFERENCE gpuPreference, bool useWarp)
	{
		uint32_t dxgiFactoryFlags{ 0 };

#ifdef BR_DEBUG
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		std::shared_ptr<GraphicsAdapter> adapter;

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory6;
		Microsoft::WRL::ComPtr<IDXGIAdapter>  dxgiAdapter;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

		ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory6.GetAddressOf())));

		if (useWarp)
		{
			ThrowIfFailed(dxgiFactory6->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter)));
			ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
		}
		else
		{
			for (uint32_t adapterIndex = 0; dxgiFactory6->EnumAdapterByGpuPreference(adapterIndex, gpuPreference, IID_PPV_ARGS(&dxgiAdapter)) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
			{
				if (SUCCEEDED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device),
					nullptr)))
				{
					ThrowIfFailed(dxgiAdapter.As(&dxgiAdapter4));
#ifdef BR_DEBUG
					DXGI_ADAPTER_DESC1 desc;
					ThrowIfFailed(dxgiAdapter4->GetDesc1(&desc));

					wchar_t buff[256] = {};
					swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
					OutputDebugStringW(buff);
#endif
					break;
				}
			}
		}

		if (dxgiAdapter4)
		{
			adapter = std::make_shared<GraphicsAdapter>(dxgiAdapter4);
		}

		return adapter;
	}

	GraphicsAdapter::GraphicsAdapter(Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter) :
		m_dxgiAdapter(dxgiAdapter)
	{
		m_dxgiAdapter->GetDesc3(&m_adapterDesc);
	}

	IDXGIAdapter4* GraphicsAdapter::GetHandle()
	{
		return m_dxgiAdapter.Get();
	}
}