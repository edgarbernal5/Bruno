#pragma once

#include "TrioApiDecl.h"

#include <vector>
#include "SurfaceFormat.h"
#include "DisplayMode.h"

namespace TrioEngine
{
	class TRIOAPI_DECL GraphicsAdapter
	{
	public:
		GraphicsAdapter();
#ifdef TRIO_DIRECTX
		GraphicsAdapter(IDXGIAdapter1* adapter, uint32_t adapterIndex);
#endif
		~GraphicsAdapter();

#ifdef TRIO_DIRECTX
		IDXGIAdapter1* GetD3DAdapter() { return m_pAdapter; }
#endif
		inline const char* GetDeviceName() { return m_csDeviceName.c_str(); }
		DisplayMode GetCurrentDisplayMode();

		const std::vector<DisplayMode>& GetSupportedDisplayModes() { return m_modes; }

		inline bool GetIsDefaultAdapter() { return m_bIsDefaultAdapter; }
		static GraphicsAdapter* GetDefaultAdapter();
		static std::vector<GraphicsAdapter*>& GetAdapters();

	private:
		std::string			m_csDeviceName;
		bool				m_bIsDefaultAdapter;
#ifdef TRIO_DIRECTX
		IDXGIAdapter1*		m_pAdapter;

		DXGI_ADAPTER_DESC1	m_stAdapter_desc;
#endif
		std::vector<DisplayMode> m_modes;
		DisplayMode			m_currentDisplayMode;

		static std::vector<GraphicsAdapter*> m_vAdapters;
		static void PopulateAdapters();
	};

}
