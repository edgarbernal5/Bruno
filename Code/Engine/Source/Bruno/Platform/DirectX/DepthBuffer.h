#pragma once

#include "D3DCommon.h"
#include "Resources.h"
#include "Texture.h"

namespace Bruno
{
	class DepthBuffer : public Texture
	{
	public:
		DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);
		~DepthBuffer();

		constexpr const DescriptorHandle& GetDsvHandle() { return mDSVDescriptor; }

	private:
		//const DescriptorHandle*					m_dsvHandle{};
		//std::unique_ptr<Texture>				m_texture{};
	};

}
