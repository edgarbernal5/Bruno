#pragma once

#include <d3d12.h>
#include "D3DConstants.h"
#include "Bruno/Core/RTTI.h"
#include "Bruno/Content/Asset.h"

namespace D3D12MA
{
	class Allocator;
	class Allocation;
}

namespace Bruno
{
	enum class GPUResourceType : uint8_t
	{
		Buffer = 0,
		Texture
	};

	class Resource : public Asset
	{
		BR_RTTI_DECLARATION(Resource, Asset);

	public:
		virtual ~Resource();

		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() { return m_virtualAddress; }
		constexpr ID3D12Resource* GetResource() { return m_resource; }
		bool IsReady() { return m_isReady; }

		friend class Surface;
		friend class Context;
		friend class GraphicsContext;
		friend class UploadContext;

	protected:
		ID3D12Resource* m_resource;
		D3D12_RESOURCE_DESC m_desc{};
		D3D12_GPU_VIRTUAL_ADDRESS m_virtualAddress = 0;
		D3D12_RESOURCE_STATES m_state = D3D12_RESOURCE_STATE_COMMON;
		uint32_t m_descriptorHeapIndex = Graphics::Core::INVALID_RESOURCE_TABLE_INDEX;
		D3D12MA::Allocation* m_allocation = nullptr;
		bool m_isReady = false;

		GPUResourceType m_resourceType = GPUResourceType::Buffer;
	};
}

