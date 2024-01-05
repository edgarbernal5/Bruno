#pragma once

#ifdef BR_PLATFORM_WINDOWS

//#define NOMINMAX
#include <Windows.h>

//#include <wrl/client.h>
#endif

//STL Headers
#include <vector>
#include <string>
#include <exception>
#include <mutex>
#include <iostream>
#include <unordered_map>

#include "Bruno/Core/Game.h"
#include "Bruno/Core/Log.h"
#include "Bruno/Core/Assert.h"

//#include "Bruno/Math/Math.h"
#include "Bruno/Renderer/PrimitiveBatch.h"