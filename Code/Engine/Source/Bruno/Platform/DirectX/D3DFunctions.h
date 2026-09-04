#pragma once

#include <d3d12.h>
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno::D3DFunctions
{
    DXGI_FORMAT GetDX12VertexFormat(VertexFormat format);
    D3D12_RASTERIZER_DESC GetDX12RasterizerState(const RasterizerState& rasterizerState);
    D3D12_BLEND_DESC GetDX12BlendState(BlendMode mode);
    D3D12_DEPTH_STENCIL_DESC GetDX12DepthState(DepthMode mode, ComparisonFunc func);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology);
    DXGI_FORMAT GetDX12Format(TextureFormat format);
    constexpr D3D12_COMPARISON_FUNC GetDX12ComparisonFunc(ComparisonFunc func);
    
    D3D12_FILTER GetDX12Filter(TextureFilter filter);
    D3D12_CULL_MODE GetDX12CullMode(CullMode mode);
    D3D12_SHADER_VISIBILITY GetDX12Visibility(ShaderVisibility visibility);
    D3D12_TEXTURE_ADDRESS_MODE GetDX12AddressMode(TextureAddressMode mode);
    D3D12_ROOT_SIGNATURE_FLAGS GetDX12RootSignatureFlags(RootSignatureFlags flags);
    
    D3D12_RESOURCE_STATES GetDX12ResourceState(ResourceState state);
}
