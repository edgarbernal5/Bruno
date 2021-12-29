#include "stdafx.h"
#include "GraphicsContentHelper.h"

#include "ContentManager.h"
#include "ContentReader.h"
#include "Graphics/IGraphicsDeviceService.h"
#include "IServiceProvider.h"

namespace BrunoEngine
{
	GraphicsDevice* GraphicsContentHelper::GraphicsDeviceFromContentReader(ContentReader* contentReader)
	{
		if (contentReader->GetContentManager()->GetServiceProvider() == nullptr)
		{
			GraphicsDevice* graphicsDevice = contentReader->GetContentManager()->GetGraphicsDevice();
			if (graphicsDevice == nullptr)
			{
				throw std::exception("FrameworkResources.NoGraphicsDeviceContent");
			}
			return graphicsDevice;
		}
		IGraphicsDeviceService* service = dynamic_cast<IGraphicsDeviceService*>(contentReader->GetContentManager()->GetServiceProvider()->GetService(typeid(IGraphicsDeviceService).name()));
		if (service == nullptr)
		{
			throw std::exception("FrameworkResources.NoGraphicsDeviceContent");
		}
		GraphicsDevice* graphicsDevice = service->GetGraphicsDevice();
		if (graphicsDevice == nullptr)
		{
			throw std::exception("FrameworkResources.NoGraphicsDeviceContent");
		}
		return graphicsDevice;
	}
}