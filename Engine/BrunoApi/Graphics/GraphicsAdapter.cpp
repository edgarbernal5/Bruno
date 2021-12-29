#include "stdafx.h"
#include "GraphicsAdapter.h"

namespace BrunoEngine
{
	std::vector<GraphicsAdapter*> GraphicsAdapter::g_Adapters;

	GraphicsAdapter::GraphicsAdapter()
	{
	}

#ifdef BRUNO_DIRECTX
	GraphicsAdapter::GraphicsAdapter(IDXGIAdapter1* adapter, uint32_t adapterIndex) :
		m_adapter(adapter)
	{
		m_isDefaultAdapter = (adapterIndex == 0);
		m_adapter->GetDesc1(&m_adapter_desc);

		char bufferDesc[128]; //TODO:
		WideCharToMultiByte(CP_ACP, 0, m_adapter_desc.Description, -1, bufferDesc, _countof(bufferDesc), NULL, FALSE);

		m_deviceName = bufferDesc;
		DXGI_FORMAT DXIGFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;
		int j = 0;
		while (adapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC outputDesc;
			pOutput->GetDesc(&outputDesc);

			if (j == 0) // Por ahora solo soportamos un output (monitor)
			{
				uint32_t numdisplayModes = 0;
				pOutput->GetDisplayModeList(DXIGFormat, 0, &numdisplayModes, nullptr);

				std::vector<DXGI_MODE_DESC> modes(numdisplayModes);
				m_modes.clear();
				pOutput->GetDisplayModeList(DXIGFormat, 0, &numdisplayModes, modes.data());

				for (auto &mode : modes)
				{
					int refreshRate = (int)(((float)mode.RefreshRate.Numerator / mode.RefreshRate.Denominator) + 0.5f);
					DisplayMode displayMode;
					displayMode.Width = mode.Width;
					displayMode.Height = mode.Height;
					displayMode.Format = SurfaceFormat::Color;

					if (std::find(m_modes.begin(), m_modes.end(), displayMode) == m_modes.end())
					{
						m_modes.push_back(displayMode);
					}
				}
			}
			j++;
		}
	}
#endif

	GraphicsAdapter::~GraphicsAdapter()
	{
	}

	std::vector<GraphicsAdapter*>& GraphicsAdapter::GetAdapters()
	{
		if (g_Adapters.size() == 0)
		{
			PopulateAdapters();
		}
		return g_Adapters;
	}

	GraphicsAdapter* GraphicsAdapter::GetDefaultAdapter()
	{
		if (g_Adapters.size() == 0)
		{
			PopulateAdapters();
		}
		return g_Adapters[0];
	}

	DisplayMode GraphicsAdapter::GetCurrentDisplayMode()
	{
		HDC hDc = GetDC(GetDesktopWindow());
		int width = GetDeviceCaps(hDc, HORZRES);
		int height = GetDeviceCaps(hDc, VERTRES);

		m_currentDisplayMode.Width = width;
		m_currentDisplayMode.Height = height;
		m_currentDisplayMode.Format = SurfaceFormat::Color;

		return m_currentDisplayMode;
	}

	void GraphicsAdapter::PopulateAdapters()
	{
#ifdef BRUNO_DIRECTX
		//TODO: EnumAdapterByGpuPreference
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
		DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

		g_Adapters.clear();

		int idx = 0;
		for (uint32_t adapterIndex = 0; dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())) != DXGI_ERROR_NOT_FOUND;
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter. Skip software devices
				continue;
			}

#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
			OutputDebugStringW(buff);
#endif

			g_Adapters.push_back(new GraphicsAdapter(adapter.Detach(), idx));
			++idx;
		}
		//TODO: fallback to dxgiFactory->EnumAdapters1 if adapter is null


#elif BRUNO_OPENGL
		//m_vAdapters.push_back(new GraphicsAdapter(0));
#endif
	}
}