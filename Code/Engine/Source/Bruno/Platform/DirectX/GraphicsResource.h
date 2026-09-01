#pragma once

#include <d3d12.h>
#include "D3DConstants.h"
#include "Bruno/Core/RTTI.h"
#include "Bruno/Content/Asset.h"
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
	enum class GPUResourceType : uint8_t
	{
		Buffer = 0,
		Texture
	};

	class GraphicsResource : public Asset
	{
		BR_RTTI_DECLARATION(GraphicsResource, Asset);

	public:
		virtual ~GraphicsResource();
		
		GraphicsResource(const GraphicsResource&) = delete;
		GraphicsResource& operator=(const GraphicsResource&) = delete;
		
		constexpr ID3D12Resource* GetResource() { return m_resource; }
	
	protected:
		ID3D12Resource* m_resource;
		ResourceState m_state = ResourceState::Common;
		std::wstring m_name;
		
	};
}

