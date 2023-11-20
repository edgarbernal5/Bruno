#pragma once

#include <exception>
#include <wrl.h>
#include <stdio.h>

namespace Bruno
{
    class com_exception : public std::exception
    {
    public:
        com_exception(HRESULT hr) noexcept : result(hr) {}

        const char* what() const override
        {
            static char s_str[64] = {};
            sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
            return s_str;
        }

    private:
        HRESULT result;
    };

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            throw com_exception(hr);
        }
    }

    template <class T>
    constexpr void SafeRelease(T*& pointer)
    {
        if (pointer)
        {
            pointer->Release();
            pointer = nullptr;
        }
    }
}