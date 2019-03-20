#pragma once

#include "TrioApiDecl.h"

namespace Vago
{
	enum class BindFlags : uint32_t
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
}
