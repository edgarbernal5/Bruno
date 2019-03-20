#pragma once

#include "TrioApiDecl.h"

#include "GraphicsDeviceInformation.h"
#include "IGraphicsDeviceManager.h"
#include "IGraphicsDeviceService.h"

#include "SurfaceFormat.h"
#include "DepthFormat.h"
#include "DisplayMode.h"

namespace Vago
{
	class Game;
	class GraphicsAdapter;
	class GraphicsDeviceManager;
	
	struct GraphicsDeviceInformationComparer
	{
		GraphicsDeviceManager* graphics;

		bool operator() (const GraphicsDeviceInformation& gdi1, const GraphicsDeviceInformation& gdi2);

	private:
		int SurfaceFormatBitDepth(SurfaceFormat format);

		int RankFormat(SurfaceFormat format);
	};

	class TRIOAPI_DECL GraphicsDeviceManager : public IGraphicsDeviceManager, public IGraphicsDeviceService
	{
	public:
		GraphicsDeviceManager(Game* game);
		~GraphicsDeviceManager();

		void ApplyChanges();

		bool BeginDraw();
		void CreateDevice();
		GraphicsDevice* GetGraphicsDevice();
		void EndDraw();

		void SetAllowMultiSampling(bool valor);
		void SetBackBufferFormat(SurfaceFormat format);
		void SetDepthStencilFormat(DepthFormat format);
		void SetIsFullScreen(bool fullscreen);
		void SetPreferredBackBufferWidth(int width);
		void SetPreferredBackBufferHeight(int height);
		void SetSynchronizeWithVerticalRetrace(bool sync);

		inline bool GetIsFullScreen() { return m_bIsFullScreen; }

		inline SurfaceFormat GetPreferredBackBufferFormat() { return m_iBackBufferFormat; }
		inline int GetPreferredBackBufferWidth() { return m_iBackBufferWidth; }

		inline int GetPreferredBackBufferHeight() { return m_iBackBufferHeight; }

		static const int DefaultBackBufferHeight;
		static const int DefaultBackBufferWidth;
	private:
		Game* m_pGame;
		GraphicsDevice* m_pDevice;

		SurfaceFormat m_iBackBufferFormat;
		DepthFormat m_eDepthStencilFormat;
		bool m_bInDeviceTransition;

		void AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices);
		void AddDevices(GraphicsAdapter* adapter, const DisplayMode& mode, GraphicsDeviceInformation baseDeviceInfo, std::vector<GraphicsDeviceInformation>& foundDevices);

		void ChangeDevice(bool forceCreate);
		void CreateDevice(GraphicsDeviceInformation newInfo);
		GraphicsDeviceInformation FindBestDevice(bool anySuitableDevice);
		bool IsWindowOnAdapter(HWND windowHandle, GraphicsAdapter* adapter);
		void WindowClientSizeChanged();

		int m_iResizedBackBufferWidth;
		int m_iResizedBackBufferHeight;
		int m_iBackBufferWidth;
		int m_iBackBufferHeight;

		bool m_bBeginDrawOk;

		bool m_bIsFullScreen;
		bool m_bUseResizedBackBuffer;
		bool m_bIsDeviceDirty;

		bool m_bAllowMultiSampling;
		bool m_bSynchronizeWithVerticalRetrace;

	protected:
		virtual void RankDevices(std::vector<GraphicsDeviceInformation>& foundDevices);
	};
}

