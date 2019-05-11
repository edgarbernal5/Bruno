#include "stdafx.h"
#include "GraphicsDeviceManager.h"

#include "Game.h"
#include "GraphicsAdapter.h"
#include "GraphicsDevice.h"
#include "Screen.h"

namespace TrioEngine
{
	int GraphicsDeviceInformationComparer::SurfaceFormatBitDepth(SurfaceFormat format)
	{
		switch (format)
		{
		case SurfaceFormat::Color:
		case SurfaceFormat::Rgba1010102:
			return 0x20;

		case SurfaceFormat::Bgr565:
		case SurfaceFormat::Bgra5551:
		case SurfaceFormat::Bgra4444:
			return 0x10;
		}
		return 0;
	}

	int GraphicsDeviceInformationComparer::RankFormat(SurfaceFormat format)
	{
		if (format == graphics->GetPreferredBackBufferFormat())
		{
			return 0;
		}
		if (SurfaceFormatBitDepth(format) == SurfaceFormatBitDepth(graphics->GetPreferredBackBufferFormat()))
		{
			return 1;
		}
		return 0x7fffffff;
	}

	const int GraphicsDeviceManager::DefaultBackBufferHeight = 600;
	const int GraphicsDeviceManager::DefaultBackBufferWidth = 800;

	GraphicsDeviceManager::GraphicsDeviceManager(Game* game) :
	m_pGame(game),
		m_bInDeviceTransition(false),
		m_pDevice(nullptr),
		m_bIsFullScreen(false),
		m_bUseResizedBackBuffer(false),
		m_bSynchronizeWithVerticalRetrace(true),
		m_bAllowMultiSampling(false),
		m_bBeginDrawOk(false)
	{
		m_iBackBufferWidth = DefaultBackBufferWidth;
		m_iBackBufferHeight = DefaultBackBufferHeight;

		m_iBackBufferFormat = SurfaceFormat::Color;
		m_eDepthStencilFormat = DepthFormat::Depth24Stencil8;

		if (game->GetServices().GetService(typeid(IGraphicsDeviceManager).name()) != nullptr)
		{
			throw std::exception("A graphics device manager is already registered");
		}

		game->GetServices().AddService(typeid(IGraphicsDeviceManager).name(), this);
		game->GetServices().AddService(typeid(IGraphicsDeviceService).name(), this);

		game->GetWindow()->ClientSizeChanged += ([=]()
		{
			WindowClientSizeChanged();
		});
	}

	GraphicsDeviceManager::~GraphicsDeviceManager()
	{
		if (m_pGame)
		{
			if (dynamic_cast<IGraphicsDeviceService*>(m_pGame->GetServices().GetService(typeid(IGraphicsDeviceService).name())) == this)
			{
				m_pGame->GetServices().RemoveService(typeid(IGraphicsDeviceService).name());
			}
		}
		if (m_pDevice)
		{
			delete m_pDevice;
			m_pDevice = nullptr;
		}
	}

	void GraphicsDeviceManager::AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		HWND handle = m_pGame->GetWindow()->GetHandle();
		for (auto &adapter : GraphicsAdapter::GetAdapters())
		{
			//TO-DO: dar soporte a multiples ventanas (monitores).
			if (anySuitableDevice || IsWindowOnAdapter(handle, adapter))
			{
				GraphicsDeviceInformation gdi;

				gdi.Adapter = adapter;
				gdi.Parameters.SetIsFullScreen(m_bIsFullScreen);
				gdi.Parameters.SetHostHWND(handle);
				gdi.Parameters.SetPresentInterval(m_bSynchronizeWithVerticalRetrace ? PresentInterval::One : PresentInterval::Immediate);

				AddDevices(gdi.Adapter, gdi.Adapter->GetCurrentDisplayMode(), gdi, foundDevices);
				if (m_bIsFullScreen)
				{
					for (auto &mode : adapter->GetSupportedDisplayModes())
					{
						if ((mode.Width >= 640) && (mode.Height >= 480))
						{
							AddDevices(adapter, mode, gdi, foundDevices);
						}
					}
				}
			}
		}
	}

	void GraphicsDeviceManager::AddDevices(GraphicsAdapter* adapter, const DisplayMode& mode, GraphicsDeviceInformation baseDeviceInfo, std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		SurfaceFormat format = this->m_iBackBufferFormat;
		DepthFormat depthFormat = this->m_eDepthStencilFormat;

		if (m_bIsFullScreen)
		{
			baseDeviceInfo.Parameters.setBackBufferHeight(mode.Height);
			baseDeviceInfo.Parameters.setBackBufferWidth(mode.Width);
		}
		else if (m_bUseResizedBackBuffer)
		{
			baseDeviceInfo.Parameters.setBackBufferHeight(m_iResizedBackBufferHeight);
			baseDeviceInfo.Parameters.setBackBufferWidth(m_iResizedBackBufferWidth);
		}
		else
		{
			baseDeviceInfo.Parameters.setBackBufferHeight(m_iBackBufferHeight);
			baseDeviceInfo.Parameters.setBackBufferWidth(m_iBackBufferWidth);
		}

		baseDeviceInfo.Parameters.setSurfaceFormat(format);
		baseDeviceInfo.Parameters.setDepthStencilFormat(depthFormat);

		bool found = false;
		for (size_t i = 0; i < foundDevices.size(); i++)
		{
			if (foundDevices[i] == baseDeviceInfo)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			foundDevices.push_back(baseDeviceInfo);
		}
	}

	void GraphicsDeviceManager::ApplyChanges()
	{
		if (m_pDevice == nullptr || m_bIsDeviceDirty)
		{
			ChangeDevice(false);
		}
	}

	bool GraphicsDeviceManager::BeginDraw()
	{
		m_bBeginDrawOk = true;
		return true;
	}

	void GraphicsDeviceManager::ChangeDevice(bool forceCreate)
	{
		m_bInDeviceTransition = true;

		const char* screenDeviceName = m_pGame->GetWindow()->GetScreenDeviceName();
		int width = m_pGame->GetWindow()->GetClientBounds().width;
		int height = m_pGame->GetWindow()->GetClientBounds().height;

		GraphicsDeviceInformation graphicsDeviceInformation = FindBestDevice(forceCreate);

		bool beginScreenDevice = false;
		bool flagCreateDevice = true;

		m_pGame->GetWindow()->BeginScreenDeviceChange(graphicsDeviceInformation.Parameters.GetIsFullScreen());
		beginScreenDevice = true;
		if (!forceCreate && (m_pDevice != nullptr))
		{
			m_pDevice->Reset(graphicsDeviceInformation.Parameters, graphicsDeviceInformation.Adapter);
			flagCreateDevice = false;
		}

		if (flagCreateDevice)
		{
			CreateDevice(graphicsDeviceInformation);
		}

		PresentationParameters& presentationParameters = m_pDevice->GetPresentationParameters();
		screenDeviceName = m_pDevice->GetAdapter()->GetDeviceName();

		if (presentationParameters.GetBackBufferWidth() != 0)
		{
			width = presentationParameters.GetBackBufferWidth();
		}
		if (presentationParameters.GetBackBufferHeight() != 0)
		{
			height = presentationParameters.GetBackBufferHeight();
		}

		if (beginScreenDevice)
		{
			m_pGame->GetWindow()->EndScreenDeviceChange(screenDeviceName, width, height);
		}

		m_bIsDeviceDirty = false;
		m_bInDeviceTransition = false;
	}

	void GraphicsDeviceManager::CreateDevice()
	{
		ChangeDevice(true);
	}

	void GraphicsDeviceManager::CreateDevice(GraphicsDeviceInformation newInfo)
	{
		if (m_pDevice != nullptr)
		{
			delete m_pDevice;
			m_pDevice = nullptr;
		}

		m_pDevice = new GraphicsDevice(newInfo.Adapter, newInfo.Parameters);
		m_pDevice->DeviceLost += ([=]()
		{

		});

		DeviceCreated();
	}

	void GraphicsDeviceManager::EndDraw()
	{
		if (m_bBeginDrawOk && m_pDevice)
		{
			m_pDevice->Present();
		}
	}

	GraphicsDeviceInformation GraphicsDeviceManager::FindBestDevice(bool anySuitableDevice)
	{
		std::vector<GraphicsDeviceInformation> foundDevices;

		AddDevices(anySuitableDevice, foundDevices);
		if ((foundDevices.size() == 0) && m_bAllowMultiSampling)
		{
			m_bAllowMultiSampling = false;
			AddDevices(anySuitableDevice, foundDevices);
		}
		if (foundDevices.size() == 0)
		{
		}
		RankDevices(foundDevices);

		return foundDevices[0];
	}

	GraphicsDevice* GraphicsDeviceManager::GetGraphicsDevice()
	{
		return m_pDevice;
	}

	bool GraphicsDeviceManager::IsWindowOnAdapter(HWND windowHandle, GraphicsAdapter* adapter)
	{
#ifdef TRIO_WINDOWS
		auto screens = Screen::GetScreens();
		return true;
#else
		return true;
#endif
	}

	void GraphicsDeviceManager::RankDevices(std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		//sort.
		GraphicsDeviceInformationComparer comparer;
		comparer.graphics = this;

		std::sort(foundDevices.begin(), foundDevices.end(), comparer);
	}

	void GraphicsDeviceManager::SetAllowMultiSampling(bool valor)
	{
		m_bAllowMultiSampling = valor;
		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetBackBufferFormat(SurfaceFormat format)
	{
		m_iBackBufferFormat = format;
		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferHeight(int height)
	{
		m_iBackBufferHeight = height;
		m_bUseResizedBackBuffer = false;

		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferWidth(int width)
	{
		m_iBackBufferWidth = width;
		m_bUseResizedBackBuffer = false;

		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetDepthStencilFormat(DepthFormat format)
	{
		m_eDepthStencilFormat = format;
		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetIsFullScreen(bool fullscreen)
	{
		m_bIsFullScreen = fullscreen;
		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetSynchronizeWithVerticalRetrace(bool sync)
	{
		m_bSynchronizeWithVerticalRetrace = sync;
		m_bIsDeviceDirty = true;
	}

	void GraphicsDeviceManager::WindowClientSizeChanged()
	{
		if (!m_bInDeviceTransition && (m_pGame->GetWindow()->GetClientBounds().height != 0) || (m_pGame->GetWindow()->GetClientBounds().height != 0))
		{
			m_iResizedBackBufferHeight = m_pGame->GetWindow()->GetClientBounds().height;
			m_iResizedBackBufferWidth = m_pGame->GetWindow()->GetClientBounds().width;
			m_bUseResizedBackBuffer = true;
			ChangeDevice(false);
		}
	}

	bool GraphicsDeviceInformationComparer::operator() (const GraphicsDeviceInformation& gdi1, const GraphicsDeviceInformation& gdi2)
	{
		int num3;
		PresentationParameters presentationParameters = gdi1.Parameters;
		PresentationParameters parameters2 = gdi2.Parameters;
		if (presentationParameters.GetIsFullScreen() != parameters2.GetIsFullScreen())
		{
			if (graphics->GetIsFullScreen() != presentationParameters.GetIsFullScreen())
			{
				return false;
			}
			return true;
		}
		int num = RankFormat(presentationParameters.GetBackBufferFormat());
		int num2 = RankFormat(parameters2.GetBackBufferFormat());
		if (num != num2)
		{
			if (num >= num2)
			{
				return false;
			}
			return true;
		}
		if (presentationParameters.GetMultiSampleCount() != parameters2.GetMultiSampleCount())
		{
			if (presentationParameters.GetMultiSampleCount() <= parameters2.GetMultiSampleCount())
			{
				return false;
			}
			return true;
		}
		if ((graphics->GetPreferredBackBufferWidth() == 0) || (graphics->GetPreferredBackBufferHeight() == 0))
		{
			num3 = ((float)GraphicsDeviceManager::DefaultBackBufferWidth) / ((float)GraphicsDeviceManager::DefaultBackBufferHeight);
		}
		else
		{
			num3 = ((float)graphics->GetPreferredBackBufferWidth()) / ((float)graphics->GetPreferredBackBufferHeight());
		}
		float num4 = ((float)presentationParameters.GetBackBufferWidth()) / ((float)presentationParameters.GetBackBufferHeight());
		float num5 = ((float)parameters2.GetBackBufferWidth()) / ((float)parameters2.GetBackBufferHeight());
		float num6 = std::fabsf(num4 - num3);
		float num7 = std::fabsf(num5 - num3);
		if (fabsf(num6 - num7) > 0.2f)
		{
			if (num6 >= num7)
			{
				return false;
			}
			return true;
		}
		int num8 = 0;
		int num9 = 0;
		if (graphics->GetIsFullScreen())
		{
			if ((graphics->GetPreferredBackBufferWidth() == 0) || (graphics->GetPreferredBackBufferHeight() == 0))
			{
				GraphicsAdapter* adapter = gdi1.Adapter;
				num8 = adapter->GetCurrentDisplayMode().Width * adapter->GetCurrentDisplayMode().Height;
				GraphicsAdapter* adapter2 = gdi2.Adapter;
				num9 = adapter2->GetCurrentDisplayMode().Width * adapter2->GetCurrentDisplayMode().Height;
			}
			else
			{
				num8 = num9 = graphics->GetPreferredBackBufferWidth() * graphics->GetPreferredBackBufferHeight();
			}
		}
		else if ((graphics->GetPreferredBackBufferWidth() == 0) || (graphics->GetPreferredBackBufferHeight() == 0))
		{
			num8 = num9 = GraphicsDeviceManager::DefaultBackBufferWidth * GraphicsDeviceManager::DefaultBackBufferHeight;
		}
		else
		{
			num8 = num9 = graphics->GetPreferredBackBufferWidth() * graphics->GetPreferredBackBufferHeight();
		}
		int num10 = fabsf((int)((presentationParameters.GetBackBufferWidth() * presentationParameters.GetBackBufferHeight()) - num8));
		int num11 = fabsf((int)((parameters2.GetBackBufferWidth() * parameters2.GetBackBufferHeight()) - num9));
		if (num10 != num11)
		{
			if (num10 >= num11)
			{
				return false;
			}
			return true;
		}
		if (gdi1.Adapter != gdi2.Adapter)
		{
			if (gdi1.Adapter->GetIsDefaultAdapter())
			{
				return true;
			}
			if (gdi2.Adapter->GetIsDefaultAdapter())
			{
				return false;
			}
		}
		return false;
	}
}