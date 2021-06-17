#pragma once

#include "TrioApiRequisites.h"
#include "SurfaceFormat.h"

namespace TrioEngine
{
	struct BRUNO_API_EXPORT DisplayMode
	{
		int Width, Height;
		SurfaceFormat Format;

		DisplayMode() : Width(0), Height(0), Format(SurfaceFormat::Color) {}
		DisplayMode(int width, int height, SurfaceFormat format) : Width(width), Height(height), Format(format) {}

		bool operator ==(const DisplayMode& d1)
		{
			return d1.Width == Width && d1.Height == Height && d1.Format == Format;
		}
	};

}
