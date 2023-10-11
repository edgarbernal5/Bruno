#include "brpch.h"
#include "DepthBuffer.h"

#include "GraphicsDevice.h"
#include "Texture.h"

namespace Bruno
{
	DepthBuffer::DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format) :
		Texture(TextureCreationDesc::Create(width, height, format, TextureViewFlags::Srv | TextureViewFlags::Dsv))
	{
	}

	DepthBuffer::~DepthBuffer()
	{
	}
}
