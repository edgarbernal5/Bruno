#include "brpch.h"
#include "VertexTypes.h"

namespace Bruno
{
    const D3D12_INPUT_ELEMENT_DESC VertexPosition::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC VertexPosition::InputLayout = {
        VertexPosition::InputElements,
        VertexPosition::InputElementCount
    };

    const D3D12_INPUT_ELEMENT_DESC VertexPositionColor::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC VertexPositionColor::InputLayout = {
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount
    };
}