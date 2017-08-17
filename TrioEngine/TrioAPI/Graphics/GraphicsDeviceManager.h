#pragma once

#include "TrioAPI.h"


#include "IGraphicsDeviceManager.h"
#include "IGraphicsDeviceService.h"
#include "DisplayMode.h"

#include <string>
#include <vector>

namespace Cuado
{
	class TRIOAPI_DLL Game;
	struct TRIOAPI_DLL GraphicsDeviceInformation;
	struct TRIOAPI_DLL GraphicsAdapter;

	class TRIOAPI_DLL GraphicsDeviceManager : public IGraphicsDeviceManager, public IGraphicsDeviceService
	{
	public:
		GraphicsDeviceManager(Game* game);
		~GraphicsDeviceManager();

		void ApplyChanges();

		bool BeginDraw();
		void CreateDevice();
		void EndDraw();

		GraphicsDevice* GetGraphicsDevice();

		void SetPreferredBackBufferWidth(int width);
		void SetPreferredBackBufferHeight(int height);

		void SetBackBufferFormat(SurfaceFormat format);
		void SetDepthStencilFormat(DepthFormat format);

		void SetIsFullScreen(bool fullscreen);
		void SetSynchronizeWithVerticalRetrace(bool sync);
		void SetAllowMultiSampling(bool valor);

		inline bool GetIsFullScreen() { return m_isFullScreen;}

		inline SurfaceFormat GetPreferredBackBufferFormat() { return m_backBufferFormat; }
		inline int GetPreferredBackBufferWidth() { return m_backBufferWidth; }

		inline int GetPreferredBackBufferHeight() { return m_backBufferHeight; }

		static const int DefaultBackBufferHeight;
		static const int DefaultBackBufferWidth;
	private:
		Game* m_game;

		bool m_inDeviceTransition;
		void WindowClientSizeChanged();

		GraphicsDevice* m_device;
		void ChangeDevice(bool forceCreate);

		int m_resizedBackBufferWidth;
		int m_resizedBackBufferHeight;
		int m_backBufferWidth;
		int m_backBufferHeight;

		SurfaceFormat m_backBufferFormat;
		DepthFormat m_depthStencilFormat;

		bool m_beginDrawOk;

		bool m_isFullScreen;
		bool m_useResizedBackBuffer;
		bool m_isDeviceDirty;

		bool m_allowMultiSampling;
		bool m_synchronizeWithVerticalRetrace;
		void CreateDevice(GraphicsDeviceInformation newInfo);

		void AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices);
		void AddDevices(GraphicsAdapter* adapter, const DisplayMode& mode, GraphicsDeviceInformation baseDeviceInfo, std::vector<GraphicsDeviceInformation>& foundDevices);

		GraphicsDeviceInformation FindBestDevice(bool anySuitableDevice);
	protected:
		virtual void RankDevices(std::vector<GraphicsDeviceInformation>& foundDevices);
	};
}