#pragma once

#include "TrioApiDecl.h"

#include "GraphicsDeviceInformation.h"
#include "IGraphicsDeviceManager.h"
#include "IGraphicsDeviceService.h"

#include "SurfaceFormat.h"
#include "DepthFormat.h"
#include "DisplayMode.h"

namespace TrioEngine
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

		inline bool GetIsFullScreen() { return m_isFullScreen; }

		inline SurfaceFormat GetPreferredBackBufferFormat() { return m_backBufferFormat; }
		inline int GetPreferredBackBufferWidth() { return m_backBufferWidth; }

		inline int GetPreferredBackBufferHeight() { return m_backBufferHeight; }

		static const int DefaultBackBufferHeight;
		static const int DefaultBackBufferWidth;
	private:
		Game* m_game;
		GraphicsDevice* m_device;

		SurfaceFormat m_backBufferFormat;
		DepthFormat m_depthStencilFormat;
		bool m_inDeviceTransition;

		void AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices);
		void AddDevices(GraphicsAdapter* adapter, const DisplayMode& mode, GraphicsDeviceInformation baseDeviceInfo, std::vector<GraphicsDeviceInformation>& foundDevices);

		void ChangeDevice(bool forceCreate);
		void CreateDevice(GraphicsDeviceInformation newInfo);
		GraphicsDeviceInformation FindBestDevice(bool anySuitableDevice);
		bool IsWindowOnAdapter(HWND windowHandle, GraphicsAdapter* adapter);
		void WindowClientSizeChanged();

		int m_resizedBackBufferWidth;
		int m_resizedBackBufferHeight;
		int m_backBufferWidth;
		int m_backBufferHeight;

		bool m_beginDrawOk;

		bool m_isFullScreen;
		bool m_useResizedBackBuffer;
		bool m_isDeviceDirty;

		bool m_allowMultiSampling;
		bool m_synchronizeWithVerticalRetrace;

	protected:
		virtual void RankDevices(std::vector<GraphicsDeviceInformation>& foundDevices);
	};
}

