#pragma once

namespace Cuado
{
	enum class DepthFormat
	{
		None,
		Depth16,
		Depth24,
		Depth24Stencil8,

		//32 bits de floating point
		Depth32
	};

	enum class SurfaceFormat
	{
		Color = 0,
		Bgr565 = 1,
		Bgra5551 = 2,
		Bgra4444 = 3,
		Dxt1 = 4,
		Dxt3 = 5,
		Dxt5 = 6,
		NormalizedByte2 = 7,
		NormalizedByte4 = 8,
		Rgba1010102 = 9,
		Rg32 = 10,
		Rgba64 = 11,
		Alpha8 = 12,
		Single = 13,
		Vector2 = 14,
		Vector4 = 15,
		HalfSingle = 16,
		HalfVector2 = 17,
		HalfVector4 = 18,
		HdrBlendable = 19,

		// BGRA formats are required for compatibility with WPF D3DImage.
		Bgr32 = 20,     // B8G8R8X8
		Bgra32 = 21,    // B8G8R8A8

		// Good explanation of compressed formats for mobile devices (aimed at Android, but describes PVRTC)
		// http://developer.motorola.com/docstools/library/understanding-texture-compression/

		// PowerVR texture compression (iOS and Android)
		RgbPvrtc2Bpp = 50,
		RgbPvrtc4Bpp = 51,
		RgbaPvrtc2Bpp = 52,
		RgbaPvrtc4Bpp = 53,

		// Ericcson Texture Compression (Android)
		RgbEtc1 = 60,

		// DXT1 also has a 1-bit alpha form
		Dxt1a = 70
	};
	

	enum class PresentInterval
	{
		One = 1,
		Two = 2,
		Immediate = 3,

		Default = 0,
	};
}