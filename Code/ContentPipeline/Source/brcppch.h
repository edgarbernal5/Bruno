#pragma once

#ifdef BR_PLATFORM_WINDOWS

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3d12sdklayers.h>
#include "d3dx12.h"

#define NOMINMAX
#include <windows.h>
#include <wrl/client.h>
#endif

//STL Headers
#include <vector>
#include <string>
#include <exception>
#include <mutex>
#include <iostream>
#include <unordered_map>