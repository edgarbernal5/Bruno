#include "brpch.h"
#include "Texture.h"

#include "GraphicsDevice.h"

namespace Bruno
{
    /*
    ver primal. d3d12_texture (D3D12Resources.cpp)
    */
	Texture::Texture(const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue)
	{
        auto device = Graphics::GetDevice();
        if (clearValue)
        {
            m_d3d12ClearValue = std::make_unique<D3D12_CLEAR_VALUE>(*clearValue);
        }

        ThrowIfFailed(device->GetD3DDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON, m_d3d12ClearValue.get(), IID_PPV_ARGS(&m_d3d12Resource)));

	}

    void Texture::CreateViews()
    {
        if (m_d3d12Resource)
        {
            auto device = Graphics::GetDevice();
            CD3DX12_RESOURCE_DESC desc(m_d3d12Resource->GetDesc());

            // Create RTV
            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) != 0 /* && CheckRTVSupport()*/)
            {
                //m_RenderTargetView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
                //d3d12Device->CreateRenderTargetView(m_d3d12Resource.Get(), nullptr,
                //    m_RenderTargetView.GetDescriptorHandle());
            }
            // Create DSV
            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL) != 0 /* && CheckDSVSupport()*/)
            {
                //m_DepthStencilView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
                //d3d12Device->CreateDepthStencilView(m_d3d12Resource.Get(), nullptr,
                //    m_DepthStencilView.GetDescriptorHandle());
            }
            // Create SRV
            if ((desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 /* && CheckSRVSupport() */ )
            {
                //m_ShaderResourceView = m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                //d3d12Device->CreateShaderResourceView(m_d3d12Resource.Get(), nullptr,
                //    m_ShaderResourceView.GetDescriptorHandle());
            }
            // Create UAV for each mip (only supported for 1D and 2D textures).
            if ((desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS) != 0 /*&& CheckUAVSupport()*/ &&
                desc.DepthOrArraySize == 1)
            {
                //m_UnorderedAccessView =
                //    m_Device.AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, desc.MipLevels);
                /*for (int i = 0; i < desc.MipLevels; ++i)
                {
                    auto uavDesc = GetUAVDesc(desc, i);
                    d3d12Device->CreateUnorderedAccessView(m_d3d12Resource.Get(), nullptr, &uavDesc,
                        m_UnorderedAccessView.GetDescriptorHandle(i));
                }*/
            }
        }
    }
}