#pragma once

namespace Cuado
{
	template <class T>
	struct Rect
	{
		T X, Y, Width, Height;
		Rect() : X(0), Y(0), Width(0), Height(0)
		{}
	};

	struct RectangleF : Rect<float>
	{
	};

	struct Rectangle : Rect<int>
	{
	};
}
