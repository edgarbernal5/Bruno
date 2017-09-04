#include "stdafx.h"
#include "GraphicsDeviceManager.h"

#include "GraphicsDevice.h"
#include "GraphicsAdapter.h"
#include "Game.h"
#include "GraphicsDeviceInformation.h"
#include "GameWindow.h"

namespace Cuado
{
	const int GraphicsDeviceManager::DefaultBackBufferHeight = 600;
	const int GraphicsDeviceManager::DefaultBackBufferWidth = 800;

	GraphicsDeviceManager::GraphicsDeviceManager(Game* game) :
		m_game(game),
		m_inDeviceTransition(false),
		m_device(nullptr),
		m_isFullScreen(false),
		m_useResizedBackBuffer(false),
		m_synchronizeWithVerticalRetrace(true),
		m_allowMultiSampling(false), m_beginDrawOk(false)
	{

		m_backBufferWidth = DefaultBackBufferWidth;
		m_backBufferHeight = DefaultBackBufferHeight;

		m_backBufferFormat = SurfaceFormat::Color;
		m_depthStencilFormat = DepthFormat::Depth24Stencil8;

		if (game->GetServices().GetService(typeid(IGraphicsDeviceManager).name()) != nullptr)
		{
			//throw
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

	void GraphicsDeviceManager::ApplyChanges()
	{
		if (m_device == nullptr || m_isDeviceDirty)
		{
			ChangeDevice(false);
		}
	}
	

	void GraphicsDeviceManager::WindowClientSizeChanged()
	{
		if (!m_inDeviceTransition && (m_game->GetWindow()->GetClientBounds().height != 0) || (m_game->GetWindow()->GetClientBounds().width != 0))
		{
			m_resizedBackBufferHeight = m_game->GetWindow()->GetClientBounds().height;
			m_resizedBackBufferWidth = m_game->GetWindow()->GetClientBounds().width;
			m_useResizedBackBuffer = true;
			ChangeDevice(false);
		}
	}

	void GraphicsDeviceManager::ChangeDevice(bool forceCreate)
	{
		m_inDeviceTransition = true;

		std::string screenDeviceName = m_game->GetWindow()->GetScreenDeviceName();
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

	void GraphicsDeviceManager::RankDevices(std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		//sort.
		GraphicsDeviceInformationComparer comparer;
		comparer.graphics = this;

		std::sort(foundDevices.begin(), foundDevices.end(), comparer);
	}


	void GraphicsDeviceManager::AddDevices(bool anySuitableDevice, std::vector<GraphicsDeviceInformation>& foundDevices)
	{
		bool multiMonitorSupport = GetSystemMetrics(SM_CMONITORS) != 0;
		HWND handle = m_game->GetWindow()->GetHandle();
		for (auto &adapter : GraphicsAdapter::GetAdapters())
		{
			//TO-DO: dar soporte a multiples ventanas (monitores).
			if (anySuitableDevice || true)
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
			baseDeviceInfo.Parameters.setBackBufferHeight(mode.Height);
			baseDeviceInfo.Parameters.setBackBufferWidth(mode.Width);
		}
		else if (m_useResizedBackBuffer)
		{
			baseDeviceInfo.Parameters.setBackBufferHeight(m_resizedBackBufferHeight);
			baseDeviceInfo.Parameters.setBackBufferWidth(m_resizedBackBufferWidth);
		}
		else
		{
			baseDeviceInfo.Parameters.setBackBufferHeight(m_backBufferHeight);
			baseDeviceInfo.Parameters.setBackBufferWidth(m_backBufferWidth);
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

	bool GraphicsDeviceManager::BeginDraw()
	{
		m_beginDrawOk = true;
		return true;
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

	void GraphicsDeviceManager::CreateDevice()
	{
		ChangeDevice(true);
	}

	void GraphicsDeviceManager::EndDraw()
	{
		if (m_beginDrawOk && m_device)
		{
			m_device->Present();
		}
	}

	GraphicsDevice* GraphicsDeviceManager::GetGraphicsDevice()
	{
		return m_device;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferWidth(int width)
	{
		m_backBufferWidth = width;
		m_useResizedBackBuffer = false;

		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetPreferredBackBufferHeight(int height)
	{
		m_backBufferHeight = height;
		m_useResizedBackBuffer = false;

		m_isDeviceDirty = true;
	}

	void GraphicsDeviceManager::SetBackBufferFormat(SurfaceFormat format)
	{
		m_backBufferFormat = format;
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

	void GraphicsDeviceManager::SetAllowMultiSampling(bool valor)
	{
		m_allowMultiSampling = valor;
		m_isDeviceDirty = true;
	}
}