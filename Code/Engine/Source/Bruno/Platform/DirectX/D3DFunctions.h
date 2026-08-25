#pragma once

#include <d3d12.h>
#include "Bruno/Renderer/RHITypes.h"

namespace Bruno::D3DFunctions
{
    DXGI_FORMAT GetDX12VertexFormat(VertexFormat format);
    D3D12_RASTERIZER_DESC GetDX12RasterizerState(CullMode cull, FillMode fill);
    D3D12_BLEND_DESC GetDX12BlendState(BlendMode mode);
    D3D12_DEPTH_STENCIL_DESC GetDX12DepthState(DepthMode mode, ComparisonFunc func);
    D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(PrimitiveTopology topology);
    DXGI_FORMAT GetDX12Format(TextureFormat format);
    constexpr D3D12_COMPARISON_FUNC GetDX12ComparisonFunc(ComparisonFunc func);
}
