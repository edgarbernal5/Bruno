#pragma once

#include "TrioAPI.h"

#include "DeviceFormats.h"

namespace Cuado
{
	struct TRIOAPI_DLL DisplayMode
	{
		int Width, Height;
		SurfaceFormat Format;

		DisplayMode() {}
		DisplayMode(int width, int height, SurfaceFormat format) : Width(width), Height(height), Format(format) {}

		bool operator ==(const DisplayMode& d1) {
			return d1.Width == Width && d1.Height == Height && d1.Format == Format;
		}
	};
}