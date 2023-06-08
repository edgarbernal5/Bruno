#pragma once

#include "D3DCommon.h"
#include "Resources.h"

namespace Bruno
{
	class DepthBuffer
	{
	public:
		DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT);
		~DepthBuffer();

		constexpr const DescriptorHandle& GetDsv() { return m_dsvHandle; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
		DescriptorHandle   m_dsvHandle{};
	};

}
