#pragma once


namespace Cuado
{
	enum class ResourceUsage
	{
		//Lectura y escritura en la GPU (ej: Depth buffer)
		Default,

		//El contenido del buffer no cambiará luego de crearse.
		Immutable,

		//A resource that is accessible by both the GPU (read only) and the CPU (write only). A dynamic resource is a good choice for a resource that will be updated by the CPU at least once per frame
		Dynamic,

		//A resource that supports data transfer (copy) from the GPU to the CPU.
		Staging
	};

	enum class IndexElementSize
	{
		SixteenBits,
		ThirtyTwoBits
	};

	enum class SetDataOptions
	{
		None,
		Discard = 1,
		NoOverwrite = 2
	};
	DEFINE_ENUM_FLAG_OPERATORS(SetDataOptions);

	enum class CpuAccessFlags
	{
		None = 0,
		Write = 65536,
		Read = 131072,
	};
	DEFINE_ENUM_FLAG_OPERATORS(CpuAccessFlags);


	enum class BindFlags
	{
		None = 0,
		VertexBuffer = 1,
		IndexBuffer = 2,
		ConstantBuffer = 4,
		ShaderResource = 8,
		StreamOutput = 16,
		RenderTarget = 32,
		DepthStencil = 64,
		UnorderedAccess = 128,
		Decoder = 512,
		VideoEncoder = 1024
	};
	DEFINE_ENUM_FLAG_OPERATORS(BindFlags);

	enum class ResourceOptionFlags
	{
		None = 0,
		GenerateMipMaps = 1,
		Shared = 2,
		TextureCube = 4,
		DrawIndirectArguments = 16,
		BufferAllowRawViews = 32,
		BufferStructured = 64,
		ResourceClamp = 128,
		SharedKeyedmutex = 256,
		GdiCompatible = 512,
		SharedNthandle = 2048,
		RestrictedContent = 4096,
		RestrictSharedResource = 8192,
		RestrictSharedResourceDriver = 16384,
		Guarded = 32768
	};
	DEFINE_ENUM_FLAG_OPERATORS(ResourceOptionFlags);

	enum class StandardMultisampleQualityLevels
	{
		// Resumen:
		//      Pattern where all of the samples are located at the pixel center.
		CenterMultisamplePattern = -2,
		//
		// Resumen:
		//      Pre-defined multi-sample patterns required for DX11 and DX10.1 hardware.
		StandardMultisamplePattern = -1,
	};
	enum class DepthStencilViewFlags
	{
		// Resumen:
		//     None.
		None = 0,
		//
		// Resumen:
		//      Indicates that depth values are read only.
		ReadOnlyDepth = 1,
		//
		// Resumen:
		//      Indicates that stencil values are read only.
		ReadOnlyStencil = 2,
	};

	enum class DepthStencilViewDimension
	{
		// Resumen:
		//      SharpDX.Direct3D11.DepthStencilViewDimension.Unknown is not a valid value
		//     for SharpDX.Direct3D11.DepthStencilViewDescription and is not used.
		Unknown = 0,
		//
		// Resumen:
		//      The resource will be accessed as a 1D texture.
		Texture1D = 1,
		//
		// Resumen:
		//      The resource will be accessed as an array of 1D textures.
		Texture1DArray = 2,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture.
		Texture2D = 3,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures.
		Texture2DArray = 4,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture with multisampling.
		Texture2DMultisampled = 5,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures with multisampling.
		Texture2DMultisampledArray = 6,
	};

	enum class RenderTargetViewDimension
	{
		//      Do not use this value, as it will cause SharpDX.Direct3D11.Device.CreateRenderTargetView(SharpDX.Direct3D11.Resource,System.Nullable<SharpDX.Direct3D11.RenderTargetViewDescription>,SharpDX.Direct3D11.RenderTargetView)
		//     to fail.
		Unknown = 0,
		//
		// Resumen:
		//      The resource will be accessed as a buffer.
		Buffer = 1,
		//
		// Resumen:
		//      The resource will be accessed as a 1D texture.
		Texture1D = 2,
		//
		// Resumen:
		//      The resource will be accessed as an array of 1D textures.
		Texture1DArray = 3,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture.
		Texture2D = 4,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures.
		Texture2DArray = 5,
		//
		// Resumen:
		//      The resource will be accessed as a 2D texture with multisampling.
		Texture2DMultisampled = 6,
		//
		// Resumen:
		//      The resource will be accessed as an array of 2D textures with multisampling.
		Texture2DMultisampledArray = 7,
		//
		// Resumen:
		//      The resource will be accessed as a 3D texture.
		Texture3D = 8
	};

	enum class ClearOptions
	{
		Target = 1,
		DepthBuffer = 2,
		Stencil = 4
	};
	DEFINE_ENUM_FLAG_OPERATORS(ClearOptions);


	enum class MapMode
	{
		Read = 1,
		Write = 2,
		ReadWrite = 3,
		WriteDiscard = 4,
		WriteNoOverwrite = 5,
	};
	DEFINE_ENUM_FLAG_OPERATORS(MapMode);

	enum class RenderTargetUsage
	{
		DiscardContents,
		PreserveContents,
		PlatformContents
	};
}