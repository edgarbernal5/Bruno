#pragma once

#include "D3DCommon.h"

namespace Bruno
{
	class ConstantBuffer
	{
	public:


	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3dBuffer;
	};
}

