//-------------------------------------------------------------------------------------
// SimpleMath.cpp -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

#include "brpch.h"



// Off by default warnings
#pragma warning(disable : 4619 4061 4265 4355 4365 4571 4623 4625 4626 4628 4668 4710 4711 4746 4774 4820 4987 5026 5027 5031 5032 5039 5045 5219 5246 5264 26812)
// C4619 #pragma warning: there is no warning number 'X'
// C4061 enumerator 'X' in switch of enum 'X' is not explicitly handled by a case label
// C4265 class has virtual functions, but destructor is not virtual
// C4355 'this': used in base member initializer list
// C4365 signed/unsigned mismatch
// C4571 behavior change
// C4623 default constructor was implicitly defined as deleted
// C4625 copy constructor was implicitly defined as deleted
// C4626 assignment operator was implicitly defined as deleted
// C4628 digraphs not supported
// C4668 not defined as a preprocessor macro
// C4710 function not inlined
// C4711 selected for automatic inline expansion
// C4746 volatile access of '<expression>' is subject to /volatile:<iso|ms> setting
// C4774 format string expected in argument 3 is not a string literal
// C4820 padding added after data member
// C4987 nonstandard extension used
// C5026 move constructor was implicitly defined as deleted
// C5027 move assignment operator was implicitly defined as deleted
// C5031/5032 push/pop mismatches in windows headers
// C5039 pointer or reference to potentially throwing function passed to extern C function under - EHc
// C5045 Spectre mitigation warning
// C5219 implicit conversion from 'int' to 'float', possible loss of data
// C5246 the initialization of a subobject should be wrapped in braces
// C5264 'const' variable is not used
// 26812: The enum type 'x' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).

// XBox One XDK related Off by default warnings
#pragma warning(disable : 4471 4643 4917 4986 5029 5043)
// C4471 forward declaration of an unscoped enumeration must have an underlying type
// C4643 Forward declaring in namespace std is not permitted by the C++ Standard
// C4917 a GUID can only be associated with a class, interface or namespace
// C4986 exception specification does not match previous declaration
// C5029 nonstandard extension used
// C5043 exception specification does not match previous declaration

#ifdef __INTEL_COMPILER
#pragma warning(disable : 161 2960 3280)
// warning #161: unrecognized #pragma
// message #2960: allocation may not satisfy the type's alignment; consider using <aligned_new> header
// message #3280: declaration hides member
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++98-compat"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#pragma clang diagnostic ignored "-Wc++98-compat-local-type-template-args"
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wglobal-constructors"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wswitch-enum"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunused-const-variable"
#pragma clang diagnostic ignored "-Wunused-member-function"
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma warning(push)
#pragma warning(disable : 4005)
#define NOMINMAX 1
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#pragma warning(pop)

#include <Windows.h>

#ifdef __MINGW32__
#include <unknwn.h>
#endif

#ifndef _WIN32_WINNT_WIN10
#define _WIN32_WINNT_WIN10 0x0A00
#endif

#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS

#ifdef _GAMING_XBOX
#include <gxdk.h>

#if _GXDK_VER < 0x55F00C58 /* GXDK Edition 220300 */
#error DirectX Tool Kit requires the March 2022 GDK or later
#endif

#ifdef _GAMING_XBOX_SCARLETT
#pragma warning(push)
#pragma warning(disable: 5204 5249)
#include <d3d12_xs.h>
#pragma warning(pop)
#include <d3dx12_xs.h>
#else
#pragma warning(push)
#pragma warning(disable: 5204)
#include <d3d12_x.h>
#pragma warning(pop)
#include <d3dx12_x.h>
#endif
#elif defined(_XBOX_ONE) && defined(_TITLE)
#include <xdk.h>

#if _XDK_VER < 0x42ED07E4 /* XDK Edition 180400  */
#error DirectX Tool Kit for Direct3D 12 requires the April 2018 XDK or later
#endif

#include <d3d12_x.h>
#include <d3dx12_x.h>
#else

#ifdef _GAMING_DESKTOP
#include <grdk.h>

#if _GRDK_VER < 0x4A611B35 /* GDK Edition 210600 */
#error DirectX Tool Kit requires June 2021 GDK or later
#endif
#endif

#ifdef USING_DIRECTX_HEADERS
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#include <dxguids/dxguids.h>
#else
#include <d3d12.h>
#endif

#include <dxgi1_4.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wtautological-type-limit-compare"
#endif

#ifdef USING_DIRECTX_HEADERS
#include <directx/d3dx12.h>
#else
#include "d3dx12.h"
#endif
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#if (defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)) || (defined(_XBOX_ONE) && defined(_TITLE))
#pragma warning(push)
#pragma warning(disable: 4471 5204 5256)
#include <Windows.UI.Core.h>
#pragma warning(pop)
#endif

#define _USE_MATH_DEFINES
#include <algorithm>
#include <atomic>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <exception>
#include <initializer_list>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 5204 5220)
#include <future>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4702)
#include <functional>
#pragma warning(pop)

#include <malloc.h>

#define _XM_NO_XMVECTOR_OVERLOADS_

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>

#if (DIRECTX_MATH_VERSION < 315)
#define XM_ALIGNED_STRUCT(x) __declspec(align(x)) struct
#endif

#pragma warning(push)
#pragma warning(disable : 4467 5038 5204 5220)
#ifdef __MINGW32__
#include <wrl/client.h>
#else
#include <wrl.h>
#endif
#pragma warning(pop)

#include <wincodec.h>

#if defined(NTDDI_WIN10_FE) || defined(__MINGW32__)
#include <ocidl.h>
#else
#include <OCIdl.h>
#endif

#ifndef __MINGW32__
// DirectX Tool Kit for Audio is in all versions of DirectXTK12
#include <mmreg.h>
#include <Audioclient.h>

#ifndef XAUDIO2_HELPER_FUNCTIONS
#define XAUDIO2_HELPER_FUNCTIONS
#endif

#include <xaudio2.h>
#include <xaudio2fx.h>

#pragma warning(push)
#pragma warning(disable : 4619 4616 5246)
#include <x3daudio.h>
#pragma warning(pop)

#include <xapofx.h>

#if (defined(_XBOX_ONE) && defined(_TITLE)) || defined(_GAMING_XBOX)
#include <apu.h>
#include <shapexmacontext.h>
#include <xma2defs.h>
#endif
#endif

#include "SimpleMath.h"

/****************************************************************************
 *
 * Constants
 *
 ****************************************************************************/

namespace DirectX
{
    namespace SimpleMath
    {
        const Vector2 Vector2::Zero = { 0.f, 0.f };
        const Vector2 Vector2::One = { 1.f, 1.f };
        const Vector2 Vector2::UnitX = { 1.f, 0.f };
        const Vector2 Vector2::UnitY = { 0.f, 1.f };

        const Vector3 Vector3::Zero = { 0.f, 0.f, 0.f };
        const Vector3 Vector3::One = { 1.f, 1.f, 1.f };
        const Vector3 Vector3::UnitX = { 1.f, 0.f, 0.f };
        const Vector3 Vector3::UnitY = { 0.f, 1.f, 0.f };
        const Vector3 Vector3::UnitZ = { 0.f, 0.f, 1.f };
        const Vector3 Vector3::Up = { 0.f, 1.f, 0.f };
        const Vector3 Vector3::Down = { 0.f, -1.f, 0.f };
        const Vector3 Vector3::Right = { 1.f, 0.f, 0.f };
        const Vector3 Vector3::Left = { -1.f, 0.f, 0.f };
        const Vector3 Vector3::Forward = { 0.f, 0.f, -1.f };
        const Vector3 Vector3::Backward = { 0.f, 0.f, 1.f };

        const Vector4 Vector4::Zero = { 0.f, 0.f, 0.f, 0.f };
        const Vector4 Vector4::One = { 1.f, 1.f, 1.f, 1.f };
        const Vector4 Vector4::UnitX = { 1.f, 0.f, 0.f, 0.f };
        const Vector4 Vector4::UnitY = { 0.f, 1.f, 0.f, 0.f };
        const Vector4 Vector4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
        const Vector4 Vector4::UnitW = { 0.f, 0.f, 0.f, 1.f };

        const Matrix Matrix::Identity = { 1.f, 0.f, 0.f, 0.f,
                                          0.f, 1.f, 0.f, 0.f,
                                          0.f, 0.f, 1.f, 0.f,
                                          0.f, 0.f, 0.f, 1.f };

        const Quaternion Quaternion::Identity = { 0.f, 0.f, 0.f, 1.f };
    }
}

using namespace DirectX;
using namespace DirectX::SimpleMath;

/****************************************************************************
 *
 * Quaternion
 *
 ****************************************************************************/

void Quaternion::RotateTowards(const Quaternion& target, float maxAngle, Quaternion& result) const noexcept
{
    const XMVECTOR T = XMLoadFloat4(this);

    // We can use the conjugate here instead of inverse assuming q1 & q2 are normalized.
    const XMVECTOR R = XMQuaternionMultiply(XMQuaternionConjugate(T), target);

    const float rs = XMVectorGetW(R);
    const XMVECTOR L = XMVector3Length(R);
    const float angle = 2.f * atan2f(XMVectorGetX(L), rs);
    if (angle > maxAngle)
    {
        const XMVECTOR delta = XMQuaternionRotationAxis(R, maxAngle);
        const XMVECTOR Q = XMQuaternionMultiply(delta, T);
        XMStoreFloat4(&result, Q);
    }
    else
    {
        // Don't overshoot.
        result = target;
    }
}

void Quaternion::FromToRotation(const Vector3& fromDir, const Vector3& toDir, Quaternion& result) noexcept
{
    // Melax, "The Shortest Arc Quaternion", Game Programming Gems, Charles River Media (2000).

    const XMVECTOR F = XMVector3Normalize(fromDir);
    const XMVECTOR T = XMVector3Normalize(toDir);

    const float dot = XMVectorGetX(XMVector3Dot(F, T));
    if (dot >= 1.f)
    {
        result = Identity;
    }
    else if (dot <= -1.f)
    {
        XMVECTOR axis = XMVector3Cross(F, Vector3::Right);
        if (XMVector3NearEqual(XMVector3LengthSq(axis), g_XMZero, g_XMEpsilon))
        {
            axis = XMVector3Cross(F, Vector3::Up);
        }

        const XMVECTOR Q = XMQuaternionRotationAxis(axis, XM_PI);
        XMStoreFloat4(&result, Q);
    }
    else
    {
        const XMVECTOR C = XMVector3Cross(F, T);
        XMStoreFloat4(&result, C);

        //const float s = sqrtf((1.f + dot) * 2.f);
        const float s = 2.f;
        result.x /= s;
        result.y /= s;
        result.z /= s;
        result.w = s * 0.5f;
    }
}

void Quaternion::LookRotation(const Vector3& forward, const Vector3& up, Quaternion& result) noexcept
{
    Quaternion q1;
    FromToRotation(Vector3::Forward, forward, q1);

    const XMVECTOR C = XMVector3Cross(forward, up);
    if (XMVector3NearEqual(XMVector3LengthSq(C), g_XMZero, g_XMEpsilon))
    {
        // forward and up are co-linear
        result = q1;
        return;
    }

    const XMVECTOR U = XMQuaternionMultiply(q1, Vector3::Up);

    Quaternion q2;
    FromToRotation(U, up, q2);

    XMStoreFloat4(&result, XMQuaternionMultiply(q2, q1));
}


 /****************************************************************************
 *
 * Viewport
 *
 ****************************************************************************/

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
static_assert(sizeof(DirectX::SimpleMath::Viewport) == sizeof(D3D11_VIEWPORT), "Size mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, x) == offsetof(D3D11_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, y) == offsetof(D3D11_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, width) == offsetof(D3D11_VIEWPORT, Width), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, height) == offsetof(D3D11_VIEWPORT, Height), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, minDepth) == offsetof(D3D11_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, maxDepth) == offsetof(D3D11_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
static_assert(sizeof(DirectX::SimpleMath::Viewport) == sizeof(D3D12_VIEWPORT), "Size mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, x) == offsetof(D3D12_VIEWPORT, TopLeftX), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, y) == offsetof(D3D12_VIEWPORT, TopLeftY), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, width) == offsetof(D3D12_VIEWPORT, Width), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, height) == offsetof(D3D12_VIEWPORT, Height), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, minDepth) == offsetof(D3D12_VIEWPORT, MinDepth), "Layout mismatch");
static_assert(offsetof(DirectX::SimpleMath::Viewport, maxDepth) == offsetof(D3D12_VIEWPORT, MaxDepth), "Layout mismatch");
#endif

#if defined(__dxgi1_2_h__) || defined(__d3d11_x_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
RECT Viewport::ComputeDisplayArea(DXGI_SCALING scaling, UINT backBufferWidth, UINT backBufferHeight, int outputWidth, int outputHeight) noexcept
{
    RECT rct = {};

    switch (int(scaling))
    {
    case DXGI_SCALING_STRETCH:
        // Output fills the entire window area
        rct.top = 0;
        rct.left = 0;
        rct.right = outputWidth;
        rct.bottom = outputHeight;
        break;

    case 2 /*DXGI_SCALING_ASPECT_RATIO_STRETCH*/:
        // Output fills the window area but respects the original aspect ratio, using pillar boxing or letter boxing as required
        // Note: This scaling option is not supported for legacy Win32 windows swap chains
        {
            assert(backBufferHeight > 0);
            const float aspectRatio = float(backBufferWidth) / float(backBufferHeight);

            // Horizontal fill
            float scaledWidth = float(outputWidth);
            float scaledHeight = float(outputWidth) / aspectRatio;
            if (scaledHeight >= float(outputHeight))
            {
                // Do vertical fill
                scaledWidth = float(outputHeight) * aspectRatio;
                scaledHeight = float(outputHeight);
            }

            const float offsetX = (float(outputWidth) - scaledWidth) * 0.5f;
            const float offsetY = (float(outputHeight) - scaledHeight) * 0.5f;

            rct.left = static_cast<LONG>(offsetX);
            rct.top = static_cast<LONG>(offsetY);
            rct.right = static_cast<LONG>(offsetX + scaledWidth);
            rct.bottom = static_cast<LONG>(offsetY + scaledHeight);

            // Clip to display window
            rct.left = std::max<LONG>(0, rct.left);
            rct.top = std::max<LONG>(0, rct.top);
            rct.right = std::min<LONG>(outputWidth, rct.right);
            rct.bottom = std::min<LONG>(outputHeight, rct.bottom);
        }
        break;

    case DXGI_SCALING_NONE:
    default:
        // Output is displayed in the upper left corner of the window area
        rct.top = 0;
        rct.left = 0;
        rct.right = std::min<LONG>(static_cast<LONG>(backBufferWidth), outputWidth);
        rct.bottom = std::min<LONG>(static_cast<LONG>(backBufferHeight), outputHeight);
        break;
    }

    return rct;
}
#endif

RECT Viewport::ComputeTitleSafeArea(UINT backBufferWidth, UINT backBufferHeight) noexcept
{
    const float safew = (float(backBufferWidth) + 19.f) / 20.f;
    const float safeh = (float(backBufferHeight) + 19.f) / 20.f;

    RECT rct;
    rct.left = static_cast<LONG>(safew);
    rct.top = static_cast<LONG>(safeh);
    rct.right = static_cast<LONG>(float(backBufferWidth) - safew + 0.5f);
    rct.bottom = static_cast<LONG>(float(backBufferHeight) - safeh + 0.5f);

    return rct;
}
