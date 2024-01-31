#pragma once

/****************************************************************************
 *
 * Int2
 *
 ****************************************************************************/

 //------------------------------------------------------------------------------
 // Comparision operators
 //------------------------------------------------------------------------------

inline bool Int2::operator == (const Int2& V) const noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR v2 = XMLoadSInt2(&V);
    return XMVector2Equal(v1, v2);
}

inline bool Int2::operator != (const Int2& V) const noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR v2 = XMLoadSInt2(&V);
    return XMVector2NotEqual(v1, v2);
}

inline std::ostream& operator<<(std::ostream& os, const Int2& V)
{
    os << "{x=" << V.x << " / y=" << V.y << "}";
    return os;
}

//------------------------------------------------------------------------------
// Assignment operators
//------------------------------------------------------------------------------
inline Int2& Int2::operator+= (const Int2& V) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR v2 = XMLoadSInt2(&V);
    const XMVECTOR X = XMVectorAdd(v1, v2);
    XMStoreSInt2(this, X);
    return *this;
}

inline Int2& Int2::operator-= (const Int2& V) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR v2 = XMLoadSInt2(&V);
    const XMVECTOR X = XMVectorSubtract(v1, v2);
    XMStoreSInt2(this, X);
    return *this;
}

inline Int2& Int2::operator*= (const Int2& V) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR v2 = XMLoadSInt2(&V);
    const XMVECTOR X = XMVectorMultiply(v1, v2);
    XMStoreSInt2(this, X);
    return *this;
}

inline Int2& Int2::operator*= (int32_t S) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR X = XMVectorScale(v1, (float)S);
    XMStoreSInt2(this, X);
    return *this;
}

inline Int2& Int2::operator/= (int32_t S) noexcept
{
    using namespace DirectX;
    assert(S != 0.0f);
    const XMVECTOR v1 = XMLoadSInt2(this);
    const XMVECTOR X = XMVectorScale(v1, 1.0f / S);
    XMStoreSInt2(this, X);
    return *this;
}

//------------------------------------------------------------------------------
// Binary operators
//------------------------------------------------------------------------------

inline Int2 operator+ (const Int2& V1, const Int2& V2) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V1);
    const XMVECTOR v2 = XMLoadSInt2(&V2);
    const XMVECTOR X = XMVectorAdd(v1, v2);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator- (const Int2& V1, const Int2& V2) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V1);
    const XMVECTOR v2 = XMLoadSInt2(&V2);
    const XMVECTOR X = XMVectorSubtract(v1, v2);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator* (const Int2& V1, const Int2& V2) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V1);
    const XMVECTOR v2 = XMLoadSInt2(&V2);
    const XMVECTOR X = XMVectorMultiply(v1, v2);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator* (const Int2& V1, int32_t S) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V1);
    const XMVECTOR X = XMVectorScale(v1, (float)S);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator/ (const Int2& V1, const Int2& V2) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V1);
    const XMVECTOR v2 = XMLoadSInt2(&V2);
    const XMVECTOR X = XMVectorDivide(v1, v2);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator/ (const Int2& V, int32_t S) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V);
    const XMVECTOR X = XMVectorScale(v1, 1.0f / S);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}

inline Int2 operator* (int32_t S, const Int2& V) noexcept
{
    using namespace DirectX;
    const XMVECTOR v1 = XMLoadSInt2(&V);
    const XMVECTOR X = XMVectorScale(v1, (float)S);
    Int2 R;
    XMStoreSInt2(&R, X);
    return R;
}