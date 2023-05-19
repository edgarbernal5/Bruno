#pragma once

#include "D3DCommon.h"
#include "Resources.h"

namespace Bruno
{
	class Texture
	{
	public:
		Texture(const std::wstring& filename);
		Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
		std::unique_ptr<D3D12_CLEAR_VALUE>     m_d3d12ClearValue;
		DescriptorHandle m_srv;

		void CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);
	};
}
