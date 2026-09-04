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
		GraphicsResource() = default;
		virtual ~GraphicsResource() = default;
		
		GraphicsResource(const GraphicsResource&) = delete;
		GraphicsResource& operator=(const GraphicsResource&) = delete;
		
		[[nodiscard]] ResourceState GetCurrentState() const { return m_currentState; }
		void SetCurrentState(ResourceState state) { m_currentState = state; }
		
		[[nodiscard]] const std::wstring& GetName() const { return m_name; }
		
		[[nodiscard]] ID3D12Resource* GetNativeResource() const { return m_resource.Get(); }
	
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;
		ResourceState m_currentState = ResourceState::Common;
		std::wstring m_name;
		
	};
}

