
#pragma once
typedef __m128 XMVECTOR;
typedef const __m128 FXMVECTOR;
#define XM_CALLCONV __vectorcall
#define XMGLOBALCONST extern const __declspec(selectany)

__declspec(align(16)) struct GXMVECTORI32
{
	union
	{
		int32_t i[4];
		XMVECTOR v;
	};

	inline operator XMVECTOR() const { return v; }
	inline operator __m128i() const { return _mm_castps_si128(v); }
	inline operator __m128d() const { return _mm_castps_pd(v); }
};


XMGLOBALCONST GXMVECTORI32 g_XMInfinity = { 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 };
XMGLOBALCONST GXMVECTORI32 g_XMQNaN = { 0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000 };
struct GLMFLOAT2
{
	float x;
	float y;

	GLMFLOAT2() {}
	GLMFLOAT2(float _x, float _y) : x(_x), y(_y) {}
	explicit GLMFLOAT2(_In_reads_(2) const float *pArray) : x(pArray[0]), y(pArray[1]) {}

	GLMFLOAT2& operator= (const GLMFLOAT2& Float2) { x = Float2.x; y = Float2.y; return *this; }
};

//------------------------------------------------------------------------------
_Use_decl_annotations_
inline __m128 __vectorcall GXMLoadFloat2
(
	const GLMFLOAT2* pSource
)
{
	__m128 x = _mm_load_ss(&pSource->x);
	__m128 y = _mm_load_ss(&pSource->y);
	return _mm_unpacklo_ps(x, y);
}

_Use_decl_annotations_
inline void __vectorcall GXMStoreFloat2
(
	GLMFLOAT2* pDestination,
	const __m128  V
)
{
	__m128 T = _mm_shuffle_ps(V, V, _MM_SHUFFLE(1, 1, 1, 1));
	_mm_store_ss(&pDestination->x, V);
	_mm_store_ss(&pDestination->y, T);
}


//------------------------------------------------------------------------------

inline bool __vectorcall GLMVector2Equal
(
	const __m128 V1,
	const __m128 V2
)
{
	__m128 vTemp = _mm_cmpeq_ps(V1, V2);
	// z and w are don't care
	return (((_mm_movemask_ps(vTemp) & 3) == 3) != 0);
}

inline bool __vectorcall GLMVector2NotEqual
(
	const __m128 V1,
	const __m128 V2
)
{
	__m128 vTemp = _mm_cmpeq_ps(V1, V2);
	// z and w are don't care
	return (((_mm_movemask_ps(vTemp) & 3) != 3) != 0);
}


//------------------------------------------------------------------------------

inline __m128 __vectorcall GLMVectorAdd
(
	const __m128 V1,
	const __m128 V2
)
{
	return _mm_add_ps(V1, V2);
}


inline __m128 __vectorcall GLMVectorSubtract
(
	const __m128 V1,
	const __m128 V2
)
{
	return _mm_sub_ps(V1, V2);
}


inline __m128 __vectorcall GLMVectorMultiply
(
	const __m128 V1,
	const __m128 V2
)
{
	return _mm_mul_ps(V1, V2);
}


inline __m128 __vectorcall GLMVectorDivide
(
	const __m128 V1,
	const __m128 V2
)
{
	return _mm_div_ps(V1, V2);
}

inline __m128 __vectorcall GLMVectorScale
(
	const __m128 V,
	float    ScaleFactor
)
{
	__m128 vResult = _mm_set_ps1(ScaleFactor);
	return _mm_mul_ps(vResult, V);
}

inline __m128 __vectorcall GXMVector2Length
(
	const __m128 V
)
{
	// Perform the dot product on x and y
	__m128 vLengthSq = _mm_mul_ps(V, V);
	// vTemp has y splatted
	__m128 vTemp = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
	// x+y
	vLengthSq = _mm_add_ss(vLengthSq, vTemp);
	vLengthSq = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
	vLengthSq = _mm_sqrt_ps(vLengthSq);
	return vLengthSq;
}

//------------------------------------------------------------------------------
// Return the X component in an FPU register. 
inline float __vectorcall GXMVectorGetX(const __m128 V)
{
	return _mm_cvtss_f32(V);
}

// Return the Y component in an FPU register. 
inline float __vectorcall GXMVectorGetY(const __m128 V)
{
	__m128 vTemp = _mm_shuffle_ps(V, V, _MM_SHUFFLE(1, 1, 1, 1));
	return _mm_cvtss_f32(vTemp);
}

// Return the Z component in an FPU register. 
inline float __vectorcall GXMVectorGetZ(const __m128 V)
{
	__m128 vTemp = _mm_shuffle_ps(V, V, _MM_SHUFFLE(2, 2, 2, 2));
	return _mm_cvtss_f32(vTemp);

}

// Return the W component in an FPU register. 
inline float __vectorcall GXMVectorGetW(const __m128 V)
{
	__m128 vTemp = _mm_shuffle_ps(V, V, _MM_SHUFFLE(3, 3, 3, 3));
	return _mm_cvtss_f32(vTemp);
}


//------------------------------------------------------------------------------
// Computation operations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

inline XMVECTOR XM_CALLCONV GXMVector2Dot
(
	FXMVECTOR V1,
	FXMVECTOR V2
)
{
	// Perform the dot product on x and y
	XMVECTOR vLengthSq = _mm_mul_ps(V1, V2);
	// vTemp has y splatted
	XMVECTOR vTemp = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
	// x+y
	vLengthSq = _mm_add_ss(vLengthSq, vTemp);
	vLengthSq = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
	return vLengthSq;

}

//------------------------------------------------------------------------------


inline XMVECTOR XM_CALLCONV GXMVector2Normalize
(
	FXMVECTOR V
)
{
	// Perform the dot product on x and y only
	XMVECTOR vLengthSq = _mm_mul_ps(V, V);
	XMVECTOR vTemp = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(1, 1, 1, 1));
	vLengthSq = _mm_add_ss(vLengthSq, vTemp);
	vLengthSq = _mm_shuffle_ps(vLengthSq, vLengthSq, _MM_SHUFFLE(0, 0, 0, 0));
	// Prepare for the division
	XMVECTOR vResult = _mm_sqrt_ps(vLengthSq);
	// Create zero with a single instruction
	XMVECTOR vZeroMask = _mm_setzero_ps();
	// Test for a divide by zero (Must be FP to detect -0.0)
	vZeroMask = _mm_cmpneq_ps(vZeroMask, vResult);
	// Failsafe on zero (Or epsilon) length planes
	// If the length is infinity, set the elements to zero
	vLengthSq = _mm_cmpneq_ps(vLengthSq, g_XMInfinity);
	// Reciprocal mul to perform the normalization
	vResult = _mm_div_ps(V, vResult);
	// Any that are infinity, set to zero
	vResult = _mm_and_ps(vResult, vZeroMask);
	// Select qnan or result based on infinite length
	XMVECTOR vTemp1 = _mm_andnot_ps(vLengthSq, g_XMQNaN);
	XMVECTOR vTemp2 = _mm_and_ps(vResult, vLengthSq);
	vResult = _mm_or_ps(vTemp1, vTemp2);
	return vResult;
}

//------------------------------------------------------------------------------

inline XMVECTOR XM_CALLCONV GXMVector2Cross
(
	FXMVECTOR V1,
	FXMVECTOR V2
)
{
	// [ V1.x*V2.y - V1.y*V2.x, V1.x*V2.y - V1.y*V2.x ]

	// Swap x and y
	XMVECTOR vResult = _mm_shuffle_ps(V2, V2, _MM_SHUFFLE(0, 1, 0, 1));
	// Perform the muls
	vResult = _mm_mul_ps(vResult, V1);
	// Splat y
	XMVECTOR vTemp = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(1, 1, 1, 1));
	// Sub the values
	vResult = _mm_sub_ss(vResult, vTemp);
	// Splat the cross product
	vResult = _mm_shuffle_ps(vResult, vResult, _MM_SHUFFLE(0, 0, 0, 0));
	return vResult;
}

//------------------------------------------------------------------------------



inline XMVECTOR XM_CALLCONV GXMVector2LengthSq
(
	FXMVECTOR V
)
{
	return GXMVector2Dot(V, V);
}