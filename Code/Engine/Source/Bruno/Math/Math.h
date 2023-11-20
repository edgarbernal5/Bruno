#pragma once

#include <d3d12.h>
#include "SimpleMath.h"

namespace Bruno::Math
{
    using BoundingBox = DirectX::BoundingBox;

    using Vector2 = DirectX::SimpleMath::Vector2;
    using Vector3 = DirectX::SimpleMath::Vector3;
    using Vector4 = DirectX::SimpleMath::Vector4;

    using Matrix = DirectX::SimpleMath::Matrix;
    using Quaternion = DirectX::SimpleMath::Quaternion;
    using Color = DirectX::SimpleMath::Color;

    using Viewport = DirectX::SimpleMath::Viewport;
    using Ray = DirectX::SimpleMath::Ray;
    using Plane = DirectX::SimpleMath::Plane;

    constexpr float ConvertToRadians(float degrees) noexcept { return degrees * (DirectX::XM_PI / 180.0f); }
    constexpr float ConvertToDegrees(float radians) noexcept { return radians * (180.0f / DirectX::XM_PI); }
    constexpr float Sign(float s) noexcept
    {
        if (s < 0) return -1.0f;
        return 1.0f;
    }

    struct Int2 : public DirectX::XMINT2
    {
        Int2() noexcept : DirectX::XMINT2(0, 0) {}
        constexpr explicit Int2(int32_t ix) noexcept : DirectX::XMINT2(ix, ix) {}
        constexpr Int2(int32_t ix, int32_t iy) noexcept : DirectX::XMINT2(ix, iy) {}
        explicit Int2(_In_reads_(2) const int32_t* pArray) noexcept : DirectX::XMINT2(pArray) {}
        Int2(DirectX::FXMVECTOR V) noexcept { XMStoreSInt2(this, V); }
        Int2(const DirectX::XMINT2& V) noexcept { this->x = V.x; this->y = V.y; }
        explicit Int2(const DirectX::XMVECTORI32& I) noexcept { this->x = I.i[0]; this->y = I.i[1]; }

        Int2(const Int2&) = default;
        Int2& operator=(const Int2&) = default;

        Int2(Int2&&) = default;
        Int2& operator=(Int2&&) = default;

        // Comparison operators
        bool operator == (const Int2& V) const noexcept;
        bool operator != (const Int2& V) const noexcept;

        // Assignment operators
        Int2& operator= (const DirectX::XMVECTORI32& I) noexcept { x = I.i[0]; y = I.i[1]; return *this; }
        Int2& operator+= (const Int2& V) noexcept;
        Int2& operator-= (const Int2& V) noexcept;
        Int2& operator*= (const Int2& V) noexcept;
        Int2& operator*= (int32_t S) noexcept;
        Int2& operator/= (int32_t S) noexcept;

        // Unary operators
        Int2 operator+ () const noexcept { return *this; }
        Int2 operator- () const noexcept { return Int2(-x, -y); }

        //friend std::ostream& operator<<(std::ostream& os, const Int2& V);
    };
    Int2 operator+ (const Int2& V1, const Int2& V2) noexcept;
    Int2 operator- (const Int2& V1, const Int2& V2) noexcept;
    Int2 operator* (const Int2& V1, const Int2& V2) noexcept;
    Int2 operator* (const Int2& V, int32_t S) noexcept;
    Int2 operator/ (const Int2& V1, const Int2& V2) noexcept;
    Int2 operator/ (const Int2& V, int32_t S) noexcept;
    Int2 operator* (int32_t S, const Int2& V) noexcept;

#include "Math.inl"
}
