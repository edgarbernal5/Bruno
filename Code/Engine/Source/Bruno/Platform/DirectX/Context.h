#pragma once

#include "D3DCommon.h"
#include "D3DCore.h"
#include <array>

namespace Bruno
{
	class Context
	{
	public:
		Context(GraphicsDevice& device, D3D12_COMMAND_LIST_TYPE commandType);
		virtual ~Context();

		D3D12_COMMAND_LIST_TYPE GetCommandType() { return mContextType; }
		ID3D12GraphicsCommandList* GetCommandList() { return mCommandList; }

	protected:
		GraphicsDevice& mDevice;

		D3D12_COMMAND_LIST_TYPE mContextType = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ID3D12GraphicsCommandList6* mCommandList{ nullptr };
		std::array<ID3D12CommandAllocator*, Graphics::Core::FRAME_BUFFER_COUNT> mCommandAllocators{ nullptr };
	};
}
