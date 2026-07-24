#pragma once

#ifdef BR_PLATFORM_WINDOWS

#define D3D12_NO_HELPERS   // evita conflictos
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
#include <memory>
#include <unordered_map>

#include "Bruno/Core/Game.h"
#include "Bruno/Core/Log.h"
#include "Bruno/Core/Assert.h"

//#include "Bruno/Math/Math.h"
//#include "Bruno/Renderer/PrimitiveBatch.h"