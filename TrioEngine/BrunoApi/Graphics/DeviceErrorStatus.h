#pragma once

#include "TrioApiRequisites.h"

namespace TrioEngine
{
	enum class DeviceErrorStatus : long
	{
		Success = 0,
#if BRUNO_DIRECTX
		Fail = E_FAIL,
		DeviceRemoved = DXGI_ERROR_DEVICE_REMOVED,
		DeviceReset = DXGI_ERROR_DEVICE_RESET,
#else
		Fail,
		DeviceRemoved,
		DeviceReset,
#endif
	};
	DEFINE_ENUM_FLAG_OPERATORS(DeviceErrorStatus);
}
