#pragma once

#include "D3DCommon.h"

namespace Bruno {

	class ResourceBarrier
	{
	public:
		constexpr static uint32_t MAX_RESOURCE_BARRIERS{ 32 };

        // Add a transition barrier to the list of barriers.
        constexpr void Add(ID3D12Resource* resource,
            D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES)
        {
            //assert(resource);
            //assert(_offset < MAX_RESOURCE_BARRIERS);
            D3D12_RESOURCE_BARRIER& barrier{ m_barriers[m_offset] };
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = flags;
            barrier.Transition.pResource = resource;
            barrier.Transition.StateBefore = before;
            barrier.Transition.StateAfter = after;
            barrier.Transition.Subresource = subresource;

            ++m_offset;
        }

        // Add a UAV barrier to the list of barriers.
        constexpr void Add(ID3D12Resource* resource,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
        {
            //assert(resource);
            //assert(_offset < MAX_RESOURCE_BARRIERS);
            D3D12_RESOURCE_BARRIER& barrier{ m_barriers[m_offset] };
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
            barrier.Flags = flags;
            barrier.UAV.pResource = resource;

            ++m_offset;
        }

        // Add an aliasing barrier to the list of barriers.
        constexpr void Add(ID3D12Resource* resourceBefore, ID3D12Resource* resourceAfter,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
        {
            //assert(resourceBefore && resourceAfter);
            //assert(_offset < MAX_RESOURCE_BARRIERS);
            D3D12_RESOURCE_BARRIER& barrier{ m_barriers[m_offset] };
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
            barrier.Flags = flags;
            barrier.Aliasing.pResourceBefore = resourceBefore;
            barrier.Aliasing.pResourceAfter = resourceAfter;

            ++m_offset;
        }

        void Apply(ID3D12GraphicsCommandList6* commandList)
        {
            //assert(_offset);
            commandList->ResourceBarrier(m_offset, m_barriers);
            m_offset = 0;
        }

        static void Transition(ID3D12GraphicsCommandList6* commandList,
            ID3D12Resource* resource,
            D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after,
            D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
            uint32_t subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES) {

            D3D12_RESOURCE_BARRIER barrier{};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = flags;
            barrier.Transition.pResource = resource;
            barrier.Transition.StateBefore = before;
            barrier.Transition.StateAfter = after;
            barrier.Transition.Subresource = subresource;

            commandList->ResourceBarrier(1, &barrier);
        }

    private:
        D3D12_RESOURCE_BARRIER      m_barriers[MAX_RESOURCE_BARRIERS]{};
        uint32_t                    m_offset{ 0 };
	};
}


