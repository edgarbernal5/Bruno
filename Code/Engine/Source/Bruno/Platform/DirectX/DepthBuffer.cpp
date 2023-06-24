#include "brpch.h"
#include "DepthBuffer.h"
#include "GraphicsDevice.h"

namespace Bruno
{
	DepthBuffer::DepthBuffer(uint32_t width, uint32_t height, DXGI_FORMAT format)
	{
		GraphicsDevice* device = Graphics::GetDevice();

		auto depthTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = format;
		optimizedClearValue.DepthStencil = { 1.0F, 0 };

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
		dsv_desc.Format = format;
		dsv_desc.Texture2D.MipSlice = 0;

		ThrowIfFailed(Graphics::GetDevice()->GetD3DDevice()->CreateCommittedResource(
			&Graphics::Core::HeapProperties.DefaultHeap,
			D3D12_HEAP_FLAG_NONE,
			&depthTextureDesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(&m_depthBuffer)
		));

		m_dsvHandle = device->GetDsvDescriptionHeap().Allocate();

		device->GetD3DDevice()->CreateDepthStencilView(m_depthBuffer.Get(), &dsv_desc,
			m_dsvHandle.Cpu);
	}

	DepthBuffer::~DepthBuffer()
	{
		//TODO: hacer release de m_dsv
	}
}
