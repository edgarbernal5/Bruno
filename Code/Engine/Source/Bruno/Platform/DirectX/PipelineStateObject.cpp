#include "brpch.h"
#include "PipelineStateObject.h"

#include "GraphicsDevice.h"

namespace Bruno
{
	PipelineStateObject::PipelineStateObject(const D3D12_PIPELINE_STATE_STREAM_DESC& desc)
	{
		auto d3d12Device = Graphics::GetDevice()->GetD3DDevice();

		ThrowIfFailed(d3d12Device->CreatePipelineState(&desc, IID_PPV_ARGS(&m_d3d12PipelineState)));
	}
}