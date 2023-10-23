#pragma once

#include "D3DCommon.h"
#include "Resources.h"
#include "Resource.h"

#include "Bruno/Core/Base.h"
#include <Bruno/Core/RTTI.h>

namespace Bruno
{
	enum class TextureViewFlags : uint8_t
	{
		None = 0,
		Rtv = 1,
		Dsv = 2,
		Srv = 4,
		Uav = 8
	};
	BR_DEFINITION_FLAG_FROM_ENUM(TextureViewFlags);

	struct TextureCreationDesc
	{
		TextureCreationDesc()
		{
			ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
			ResourceDesc.Width = 0;
			ResourceDesc.Height = 0;
			ResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
			ResourceDesc.DepthOrArraySize = 1;
			ResourceDesc.MipLevels = 1;
			ResourceDesc.SampleDesc.Count = 1;
			ResourceDesc.SampleDesc.Quality = 0;
			ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			ResourceDesc.Alignment = 0;
		}

		D3D12_RESOURCE_DESC ResourceDesc{};
		TextureViewFlags ViewFlags = TextureViewFlags::None;

		static TextureCreationDesc Create(uint32_t width, uint32_t height, DXGI_FORMAT format = DXGI_FORMAT_D32_FLOAT, TextureViewFlags viewFlags = TextureViewFlags::None)
		{
			TextureCreationDesc desc;
			desc.ResourceDesc.Format = format;
			desc.ResourceDesc.Width = width;
			desc.ResourceDesc.Height = height;
			desc.ViewFlags = viewFlags;
			return desc;
		}
	};

	class Texture;

	struct TextureUpload
	{
		Texture* Texture = nullptr;
		std::unique_ptr<uint8_t[]> TextureData;
		size_t TextureDataSize = 0;
		uint32_t SubResourcesCount = 0;
		SubResourceLayouts SubResourceLayouts{ 0 };
	};

	class Texture : public Resource
	{
		BR_RTTI_DECLARATION(Texture, Resource);

	public:
		struct AssetPipelineInitData
		{
			size_t Width;
			size_t Height;
			size_t Depth;
			size_t ArraySize;
			size_t MipLevels;
			uint32_t Format;
			uint8_t Dimension;
			uint64_t DataSizeInBytes;

			std::vector<uint8_t> Pixels;
			struct ImageInitData
			{
				int64_t RowPitch;
				int64_t SlicePitch;
				uint64_t Offset;
				//std::vector<uint8_t> Pixels;
			};
			std::vector<ImageInitData> Images;
		};
		Texture();
		Texture(const std::wstring& filename);
		Texture(const AssetPipelineInitData& textureInitData);
		Texture(const TextureCreationDesc& textureDesc);
		~Texture();

		constexpr const DescriptorHandle& GetSrvHandle() const { return m_srvDescriptor; }
		constexpr const DescriptorHandle& GetDsvHandle() const { return m_dsvDescriptor; }

		friend class Surface;
		friend class GraphicsContext;
		friend class GraphicsDevice;

	protected:
		DescriptorHandle m_rtvDescriptor{};
		DescriptorHandle m_dsvDescriptor{};
		DescriptorHandle m_srvDescriptor{};
		DescriptorHandle m_uavDescriptor{};

	private:
		void CreateTexture(const TextureCreationDesc& desc);
		void CopyTextureSubresource(uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData);
		void GenerateMips();
	};
}
