// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(push)
#pragma warning(disable : 4005)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//#define NODRAWTEXT
//#define NOGDI
//#define NOBITMAP
//#define NOMCX
//#define NOSERVICE
//#define NOHELP
#pragma warning(pop)

#include <windows.h>

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

#include <wrl/client.h>

#ifdef TRIO_DIRECTX

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#define DCOMMON_H_INCLUDED
#else
#include <d3d11_1.h>
#endif

#endif

#if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#include <Windows.UI.Core.h>
#endif

#ifdef TRIO_DIRECTX

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <D3Dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#endif

#include <algorithm>
#include <array>
#include <exception>
#include <list>
#include <malloc.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <stdint.h>

#include <wrl.h>

#include <wincodec.h>


#include "Graphics\SimpleMath.h"
#include "Graphics\GraphicsDevice.h"

#ifdef TRIO_DIRECTX
namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		virtual const char* what() const override
		{
			static char s_str[64] = { 0 };
			sprintf_s(s_str, "Failure with HRESULT of %08X", result);
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

#define ReleaseCOM(com) if(com) { com->Release(); com = nullptr; }
}
#endif