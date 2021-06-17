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
	m_game(game),
		m_inDeviceTransition(false),
		m_device(nullptr),
		m_isFullScreen(false),
		m_useResizedBackBuffer(false),
		m_synchronizeWithVerticalRetrace(true),
		m_allowMultiSampling(false),
		m_beginDrawOk(false)
	{
		m_backBufferWidth = DefaultBackBufferWidth;
		m_backBufferHeight = DefaultBackBufferHeight;

		m_backBufferFormat = SurfaceFormat::Color;
		m_depthStencilFormat = DepthFormat::Depth24Stencil8;

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
		if (m_game)
		{
			if (dynamic_cast<IGraphicsDeviceService*>(m_game->GetServices().GetService(typeid(IGraphicsDeviceService).name())) == this)
			{
				m_game->GetServices().RemoveService(typeid(IGraphicsDeviceService).name());
			}
		}
		if (m_device)
		{
			delete m_device;
			m_device = nullptr;
		}
	}

	void GraphicsDeviceManager::AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		HWND handle = m_game->GetWindow()->GetHandle();
		for (auto &adapter : GraphicsAdapter::GetAdapters())
		{
			//TO-DO: dar soporte a multiples ventanas (monitores).
			if (anySuitableDevice || IsWindowOnAdapter(handle, adapter))
			{
				GraphicsDeviceInformation gdi;

				gdi.Adapter = adapter;
				gdi.Parameters.SetIsFullScreen(m_isFullScreen);
				gdi.Parameters.SetHostHWND(handle);
				gdi.Parameters.SetPresentInterval(m_synchronizeWithVerticalRetrace ? PresentInterval::One : PresentInterval::Immediate);

				AddDevices(gdi.Adapter, gdi.Adapter->GetCurrentDisplayMode(), gdi, foundDevices);
				if (m_isFullScreen)
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
		SurfaceFormat format = this->m_backBufferFormat;
		DepthFormat depthFormat = this->m_depthStencilFormat;

		if (m_isFullScreen)
		{
			baseDeviceInfo.Parameters.SetBackBufferHeight(mode.Height);
			baseDeviceInfo.Parameters.SetBackBufferWidth(mode.Width);
		}
		else if (m_useResizedBackBuffer)
		{
			baseDeviceInfo.Parameters.SetBackBufferHeight(m_resizedBackBufferHeight);
			baseDeviceInfo.Parameters.SetBackBufferWidth(m_resizedBackBufferWidth);
		}
		else
		{
			baseDeviceInfo.Parameters.SetBackBufferHeight(m_backBufferHeight);
			baseDeviceInfo.Parameters.SetBackBufferWidth(m_backBufferWidth);
		}

		baseDeviceInfo.Parameters.GetBackBufferFormat(format);
		baseDeviceInfo.Parameters.SetDepthStencilFormat(depthFormat);

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
		if (m_device == nullptr || m_isDeviceDirty)
		{
			ChangeDevice(false);
		}
	}

	bool GraphicsDeviceManager::BeginDraw()
	{
		m_beginDrawOk = true;
		return true;
	}

	void GraphicsDeviceManager::ChangeDevice(bool forceCreate)
	{
		m_inDeviceTransition = true;

		const char* screenDeviceName = m_game->GetWindow()->GetScreenDeviceName();
		int width = m_game->GetWindow()->GetClientBounds().width;
		int height = m_game->GetWindow()->GetClientBounds().height;

		GraphicsDeviceInformation graphicsDeviceInformation = FindBestDevice(forceCreate);

		bool beginScreenDevice = false;
		bool flagCreateDevice = true;

		m_game->GetWindow()->BeginScreenDeviceChange(graphicsDeviceInformation.Parameters.GetIsFullScreen());
		beginScreenDevice = true;
		if (!forceCreate && (m_device != nullptr))
		{
			m_device->Reset(graphicsDeviceInformation.Parameters, graphicsDeviceInformation.Adapter);
			flagCreateDevice = false;
		}

		if (flagCreateDevice)
		{
			CreateDevice(graphicsDeviceInformation);
		}

		PresentationParameters& presentationParameters = m_device->GetPresentationParameters();
		screenDeviceName = m_device->GetAdapter()->GetDeviceName();

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
			m_game->GetWindow()->EndScreenDeviceChange(screenDeviceName, width, height);
		}

		m_isDeviceDirty = false;
		m_inDeviceTransition = false;
	}

	void GraphicsDeviceManager::CreateDevice()
	{
		ChangeDevice(true);
	}

	void GraphicsDeviceManager::CreateDevice(GraphicsDeviceInformation newInfo)
	{
		if (m_device != nullptr)
		{
			delete m_device;
			m_device = nullptr;
		}

		m_device = new GraphicsDevice(newInfo.Adapter, newInfo.Parameters);
		m_device->DeviceLost += ([=]()
		{
			
		});

		DeviceCreated();
	}

	void GraphicsDeviceManager::EndDraw()
	{
		if (m_beginDrawOk && m_device)
		{
			m_device->Present();
		}
	}

	GraphicsDeviceInformation GraphicsDeviceManager::FindBestDevice(bool anySuitableDevice)
	{
		std::vector<GraphicsDeviceInformation> foundDevices;

		AddDevices(anySuitableDevice, foundDevices);
		if ((foundDevices.size() == 0) && m_allowMultiSampling)
		{
			m_allowMultiSampling = false;
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
		return m_device;
	}

	bool GraphicsDeviceManager::IsWindowOnAdapter(HWND windowHandle, GraphicsAdapter* adapter)
	{
#ifdef BRUNO_WINDOWS
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
		m_allowMultiSampling = valor;
		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetBackBufferFormat(SurfaceFormat format)
	{
		m_backBufferFormat = format;
		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferHeight(int height)
	{
		m_backBufferHeight = height;
		m_useResizedBackBuffer = false;

		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferWidth(int width)
	{
		m_backBufferWidth = width;
		m_useResizedBackBuffer = false;

		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetDepthStencilFormat(DepthFormat format)
	{
		m_depthStencilFormat = format;
		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetIsFullScreen(bool fullscreen)
	{
		m_isFullScreen = fullscreen;
		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetSynchronizeWithVerticalRetrace(bool sync)
	{
		m_synchronizeWithVerticalRetrace = sync;
		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::WindowClientSizeChanged()
	{
		if (!m_inDeviceTransition && (m_game->GetWindow()->GetClientBounds().height != 0) || (m_game->GetWindow()->GetClientBounds().height != 0))
		{
			m_resizedBackBufferHeight = m_game->GetWindow()->GetClientBounds().height;
			m_resizedBackBufferWidth = m_game->GetWindow()->GetClientBounds().width;
			m_useResizedBackBuffer = true;
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