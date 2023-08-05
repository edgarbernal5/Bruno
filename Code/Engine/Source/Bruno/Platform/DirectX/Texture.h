#pragma once

#include "D3DCommon.h"
#include "Resources.h"

#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	class Texture : public RTTI
	{
		BR_RTTI_DECLARATION(Texture, RTTI);

	public:

		struct InitData
		{
			size_t Width;
			size_t Height;
			size_t Depth;
			size_t ArraySize;
			size_t MipLevels;
			uint32_t Format;
			uint8_t Dimension;

			struct ImageInitData
			{
				int64_t RowPitch;
				int64_t SlicePitch;
				std::vector<uint8_t> Pixels;
			};
			std::vector<ImageInitData> Images;
		};

		Texture(const std::wstring& filename);
		Texture(const InitData& textureInitData);

		constexpr const DescriptorHandle& GetSrvHandle() const { return m_srvHandle; }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource>	m_d3d12Resource;
		std::unique_ptr<D3D12_CLEAR_VALUE>		m_d3d12ClearValue;
		DescriptorHandle						m_srvHandle;

		void CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);
		void GenerateMips();
		DXGI_FORMAT MakeSRGB(DXGI_FORMAT fmt);
	};
}
