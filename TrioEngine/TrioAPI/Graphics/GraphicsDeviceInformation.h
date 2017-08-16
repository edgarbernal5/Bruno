#pragma once

#include "TrioAPI.h"

#include "GraphicsAdapter.h"
#include "PresentationParameters.h"

namespace Cuado
{
	struct TRIOAPI_DLL GraphicsDeviceInformation
	{
		GraphicsAdapter* Adapter;
		PresentationParameters Parameters;

		bool operator ==(const GraphicsDeviceInformation &other) const
		{
			return Parameters == other.Parameters && Adapter == other.Adapter;
		}
	};

	struct GraphicsDeviceInformationComparer
	{
		GraphicsDeviceManager* graphics;

		inline bool operator() (const GraphicsDeviceInformation& gdi1, const GraphicsDeviceInformation& gdi2)
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
			float num6 = fabsf(num4 - num3);
			float num7 = fabsf(num5 - num3);
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
					num8 = adapter->GetCurrentDisplayMode()->Width * adapter->GetCurrentDisplayMode()->Height;
					GraphicsAdapter* adapter2 = gdi2.Adapter;
					num9 = adapter2->GetCurrentDisplayMode()->Width * adapter2->GetCurrentDisplayMode()->Height;
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

	private:
		int SurfaceFormatBitDepth(SurfaceFormat format)
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

		int RankFormat(SurfaceFormat format)
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
	};
}