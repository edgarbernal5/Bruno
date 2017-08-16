#pragma once

#include "TrioAPI.h"

namespace Cuado
{
	class TRIOAPI_DLL IRenderTarget;
	class TRIOAPI_DLL RenderTarget2D;

	struct TRIOAPI_DLL RenderTargetBinding {
		IRenderTarget *RenderTarget;
		int ArraySlice;

		RenderTargetBinding() : RenderTarget(nullptr) {
			ArraySlice = 0;
		}

		RenderTargetBinding(RenderTarget2D* renderTarget) {
			RenderTarget = (IRenderTarget*)renderTarget;
			ArraySlice = 0;
		}

		RenderTargetBinding(const RenderTargetBinding &other) {
			RenderTarget = other.RenderTarget;
			ArraySlice = other.ArraySlice;
		}
	};
}