#pragma once

#include "TrioApiRequisites.h"

#include <vector>
#include "SurfaceFormat.h"
#include "DisplayMode.h"

namespace TrioEngine
{
	class BRUNO_API_EXPORT GraphicsAdapter
	{
	public:
		GraphicsAdapter();
#ifdef BRUNO_DIRECTX
		GraphicsAdapter(IDXGIAdapter1* adapter, uint32_t adapterIndex);
#endif
		~GraphicsAdapter();

#ifdef BRUNO_DIRECTX
		IDXGIAdapter1* GetD3DAdapter() { return m_adapter; }
#endif
		inline const char* GetDeviceName() { return m_deviceName.c_str(); }
		DisplayMode GetCurrentDisplayMode();

		const std::vector<DisplayMode>& GetSupportedDisplayModes() { return m_modes; }

		inline bool GetIsDefaultAdapter() { return m_isDefaultAdapter; }
		static GraphicsAdapter* GetDefaultAdapter();
		static std::vector<GraphicsAdapter*>& GetAdapters();

	private:
		std::string			m_deviceName;
		bool				m_isDefaultAdapter;
#ifdef BRUNO_DIRECTX
		IDXGIAdapter1*		m_adapter;

		DXGI_ADAPTER_DESC1	m_adapter_desc;
#endif
		std::vector<DisplayMode> m_modes;
		DisplayMode			m_currentDisplayMode;

		static std::vector<GraphicsAdapter*> g_Adapters;
		static void PopulateAdapters();
	};

}
