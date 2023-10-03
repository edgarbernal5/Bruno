#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include "Bruno/Core/RTTI.h"

namespace D3D12MA
{
	class Allocator;
	class Allocation;
}

namespace Bruno
{
	enum class GPUResourceType : bool
	{
		Buffer = false,
		Texture = true
	};

	class Resource : public RTTI
	{
	public:
		BR_RTTI_DECLARATION(Resource, RTTI);

		constexpr ID3D12Resource* GetResource() { return mResource; }

		friend class Surface;
		friend class Context;
	protected:
		ID3D12Resource* mResource;
		D3D12_RESOURCE_DESC mDesc{};
		D3D12_GPU_VIRTUAL_ADDRESS mVirtualAddress = 0;
		D3D12_RESOURCE_STATES mState = D3D12_RESOURCE_STATE_COMMON;
		uint32_t mDescriptorHeapIndex = Graphics::Core::INVALID_RESOURCE_TABLE_INDEX;
		D3D12MA::Allocation* mAllocation = nullptr;

		GPUResourceType mType = GPUResourceType::Buffer;
	};
}

