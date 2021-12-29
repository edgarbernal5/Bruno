// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
//// Windows Header Files
//#include <windows.h>

#include <WinSDKVer.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <SDKDDKVer.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
//#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <wrl/client.h>

#ifdef BRUNO_DIRECTX

#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#endif


#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

#include <stdio.h>

//DevIL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

#include "Graphics/GraphicsDevice.h"
#include "Graphics/VertexDeclaration.h"
//#include "Content/ContentManager.h"

#ifdef BRUNO_DIRECTX
#endif

// reference additional headers your program requires here
namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}

	inline void ThrowIfFailed(BrunoEngine::DeviceErrorStatus status)
	{
		if (FAILED(status))
		{
			throw com_exception((HRESULT)status);
		}
	}

}

#define DELETE_OBJECT(OBJ) if(OBJ) { delete OBJ; OBJ = nullptr; }
#define RELEASE_COM(COM) if(COM) { COM->Release(); COM = nullptr; }

#define RELEASE_PTR(OBJ) if(OBJ) { delete OBJ; OBJ = nullptr; }