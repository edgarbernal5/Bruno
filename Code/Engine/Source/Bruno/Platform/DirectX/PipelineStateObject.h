#pragma once

#include "D3DCommon.h"

namespace Bruno
{
	class PipelineStateObject
	{
	public:
		PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc);
		virtual ~PipelineStateObject() = default;
		
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_d3d12PipelineState.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_d3d12PipelineState;
	};
}

