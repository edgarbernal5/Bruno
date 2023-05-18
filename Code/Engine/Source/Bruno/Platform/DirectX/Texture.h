#pragma once

#include "D3DCommon.h"

namespace Bruno
{
	class Texture
	{
	public:
		Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
		std::unique_ptr<D3D12_CLEAR_VALUE>     m_d3d12ClearValue;

		void CreateViews();
	};
}
