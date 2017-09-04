#include "stdafx.h"
#include "GraphicsAdapter.h"

#include <wingdi.h>

using Microsoft::WRL::ComPtr;
//TODO
namespace Cuado
{
	std::vector<GraphicsAdapter*> GraphicsAdapter::m_adapters;

	GraphicsAdapter::GraphicsAdapter(uint32_t adapterIndex) 
	{
		m_isDefaultAdapter = (adapterIndex == 0);
		m_deviceName = "";

		MONITORINFOEXW target;
		target.cbSize = sizeof(MONITORINFOEXW);

		//create a monitor info struct to store the data to
		HMONITOR Hmon = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
		//create a handle to the main monitor
		//(should start at top left of screen with (0,0) as apposed to other monitors i believe)
		//if i could gather aditional info on what monitors are available that might be           useful
		GetMonitorInfoW(Hmon, &target);

		size_t size = wcslen(target.szDevice);

		std::wstring wstr(target.szDevice, target.szDevice + size);
		m_deviceName = std::string(wstr.begin(), wstr.end());

		HDC hDC = GetDC(GetDesktopWindow());
		
		DisplayMode displayMode;
		displayMode.Width = GetDeviceCaps(hDC, HORZRES);
		displayMode.Height = GetDeviceCaps(hDC, VERTRES);
		displayMode.Format = SurfaceFormat::Color;
		
		m_modes.push_back(displayMode);

#if TRIO_OPENGL
		DisplayMode currentResolution;
		bool primary = false;

		DISPLAY_DEVICE dd;

		dd.cb = sizeof(DISPLAY_DEVICE);

		std::vector<DisplayMode> availableResolutions;

		DWORD deviceNum = 0;
		while (EnumDisplayDevices(nullptr, deviceNum, &dd, 0)){

			if (dd.StateFlags & DISPLAY_DEVICE_ACTIVE)
			{
				DEVMODE deviceMode;
				ZeroMemory(&deviceMode, sizeof(DEVMODE));
				deviceMode.dmSize = sizeof(DEVMODE);

				//DISPLAY_DEVICE_ATTACHED_TO_DESKTOP
				if (EnumDisplaySettingsEx(dd.DeviceName, ENUM_CURRENT_SETTINGS, &deviceMode, 0) ||
					EnumDisplaySettingsEx(dd.DeviceName, ENUM_REGISTRY_SETTINGS, &deviceMode, 0))
				{
					if (deviceMode.dmBitsPerPel == 0)
					{
						deviceMode.dmBitsPerPel = 0x20;
					}
					float scale = GetScale(deviceMode);
					currentResolution = DisplayMode((int)(((float)deviceMode.dmPelsWidth) / scale), (int)(((float)deviceMode.dmPelsHeight) / scale), SurfaceFormat::Color);
					primary = (dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;
				}
				availableResolutions.clear();
				//DumpDevice(dd, 0);
				DISPLAY_DEVICE newdd = { 0 };
				newdd.cb = sizeof(DISPLAY_DEVICE);
				DWORD monitorNum = 0;
				while (EnumDisplaySettingsEx(dd.DeviceName, monitorNum++, &deviceMode, 0))
				{
					if (deviceMode.dmBitsPerPel == 0)
					{
						deviceMode.dmBitsPerPel = 0x20;
					}
					float num4 = GetScale(deviceMode);
					DisplayMode resolution2 = DisplayMode((int)(((float)deviceMode.dmPelsWidth) / num4), (int)(((float)deviceMode.dmPelsHeight) / num4), SurfaceFormat::Color);
					availableResolutions.push_back(resolution2);
				}

				if (primary && m_isDefaultAdapter)
				{
					m_modes = availableResolutions;
				}

				//while (EnumDisplayDevices(dd.DeviceName, monitorNum, &newdd, 0))
				//{
				//	//DumpDevice(newdd, 4);
				//	monitorNum++;
				//}

			}
			deviceNum++;
		}

#endif
	}

#if TRIO_OPENGL
	float GraphicsAdapter::GetScale(DEVMODE& monitor_mode)
	{
		float num = 1.0f;
		if ((monitor_mode.dmFields & 0x20000) != 0)
		{
			num = ((float) monitor_mode.dmLogPixels) / 96.0f;
		}
		return num;

	}
#endif

#ifdef TRIO_DIRECTX
	GraphicsAdapter::GraphicsAdapter(IDXGIAdapter1* adapter, uint32_t adapterIndex) :
		m_adapter(adapter)
	{
		m_isDefaultAdapter = (adapterIndex == 0);
		m_adapter->GetDesc1(&m_adapter_desc);

		std::wstring wstr(m_adapter_desc.Description);

		m_deviceName = std::string(wstr.begin(), wstr.end());

		IDXGIOutput *pOutput = nullptr;
		int j = 0;
		while (adapter->EnumOutputs(j, &pOutput) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_OUTPUT_DESC outputDesc;
			pOutput->GetDesc(&outputDesc);

			if (j == 0) // Por ahora solo soportamos un output (monitor)
			{
				uint32_t numdisplayModes = 0;
				pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numdisplayModes, nullptr);

				std::vector<DXGI_MODE_DESC> modes(numdisplayModes);
				m_modes.clear();
				pOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numdisplayModes, &modes[0]);

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


	GraphicsAdapter* GraphicsAdapter::GetDefaultAdapter()
	{
		if (m_adapters.size() == 0)
		{
			PopulateAdapters();
		}
		return m_adapters[0];
	}


	std::vector<GraphicsAdapter*>& GraphicsAdapter::GetAdapters()
	{
		if (m_adapters.size() == 0)
		{
			PopulateAdapters();
		}
		return m_adapters;		
	}

	void GraphicsAdapter::PopulateAdapters()
	{
#ifdef TRIO_DIRECTX
		/*
		DXGI_MODE_DESC closestMatch;
		DXCall(output->FindClosestMatchingMode(&desiredMode, &closestMatch, device.GetInterfacePtr()));
		*/

		ComPtr<IDXGIFactory1> dxgiFactory;
		DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

		ComPtr<IDXGIAdapter1> adapter;

		std::vector <IDXGIAdapter*> vAdapters;

		int idx = 0;
		for (uint32_t adapterIndex = 0; dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

#ifdef _DEBUG
			wchar_t buff[256] = {};
			swprintf_s(buff, L"Direct3D Adapter (%u): VID:%04X, PID:%04X - %ls\n", adapterIndex, desc.VendorId, desc.DeviceId, desc.Description);
			//OutputDebugStringW(buff);
#endif

			m_adapters.push_back(new GraphicsAdapter(adapter.Detach(), idx));
			++idx;
		}
		
#elif TRIO_OPENGL
		m_adapters.push_back(new GraphicsAdapter(0));
#endif
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
}