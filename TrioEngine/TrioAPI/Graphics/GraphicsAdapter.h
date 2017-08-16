#pragma once


#include "TrioAPI.h"

#include "DisplayMode.h"
#include <vector>
#include <string>

namespace Cuado
{
	class TRIOAPI_DLL GraphicsAdapter
	{
	public:
		GraphicsAdapter(uint32_t adapterIndex);
#ifdef TRIO_DIRECTX
		GraphicsAdapter(IDXGIAdapter1* adapter, uint32_t adapterIndex);
#endif
		~GraphicsAdapter();

#ifdef TRIO_DIRECTX
		IDXGIAdapter1* GetD3DAdapter() { return m_adapter; }
#endif
		inline std::string& GetDeviceName() { return m_deviceName; }
		DisplayMode* GetCurrentDisplayMode();

		std::vector<DisplayMode>& GetSupportedDisplayModes() { return m_modes; }

		inline bool GetIsDefaultAdapter() { return m_isDefaultAdapter; }
		static GraphicsAdapter* GetDefaultAdapter();
		static std::vector<GraphicsAdapter*>& GetAdapters();
	private:

		std::string			m_deviceName;
		bool				m_isDefaultAdapter;
#ifdef TRIO_DIRECTX
		IDXGIAdapter1*		m_adapter;

		DXGI_ADAPTER_DESC1	m_adapter_desc;


#elif TRIO_OPENGL
		float GetScale(DEVMODE& monitor_mode);

#endif
		std::vector<DisplayMode> m_modes;
		DisplayMode*			m_currentDisplayMode;

		static std::vector<GraphicsAdapter*> m_adapters;
		static void PopulateAdapters();
	};
}
