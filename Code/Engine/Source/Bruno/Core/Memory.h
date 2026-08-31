#pragma once

#include <cstdint>

namespace Bruno
{
    template <typename T>
    constexpr uint32_t GetAlignedConstantBufferSize()
    {
        return (sizeof(T) + 255) & ~255;
    }
    
    template <typename T>
    constexpr uint32_t GetAlignedSize(T alignment)
    {
        alignment -= 1;
        return (sizeof(T) + alignment) & ~alignment;
    }
}