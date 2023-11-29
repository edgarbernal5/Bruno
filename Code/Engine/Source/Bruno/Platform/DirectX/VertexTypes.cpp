#include "brpch.h"
#include "VertexTypes.h"

#include <functional>

namespace Bruno
{
    const size_t VertexPosition::HashId = std::hash<std::string>{}("P");

    const D3D12_INPUT_ELEMENT_DESC VertexPosition::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC& VertexPosition::InputLayout = {
        VertexPosition::InputElements,
        VertexPosition::InputElementCount
    };

    const size_t VertexPositionColor::HashId = std::hash<std::string>{}("PC");

    const D3D12_INPUT_ELEMENT_DESC VertexPositionColor::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC& VertexPositionColor::InputLayout = {
        VertexPositionColor::InputElements,
        VertexPositionColor::InputElementCount
    };

    const size_t VertexPositionNormalColor::HashId = std::hash<std::string>{}("PNC");

    const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalColor::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC& VertexPositionNormalColor::InputLayout = {
        VertexPositionNormalColor::InputElements,
        VertexPositionNormalColor::InputElementCount
    };

    const size_t VertexPositionNormalTexture::HashId = std::hash<std::string>{}("PNT");

    const D3D12_INPUT_ELEMENT_DESC VertexPositionNormalTexture::InputElements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    const D3D12_INPUT_LAYOUT_DESC& VertexPositionNormalTexture::InputLayout = {
        VertexPositionNormalTexture::InputElements,
        VertexPositionNormalTexture::InputElementCount
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unordered_map<size_t, const D3D12_INPUT_LAYOUT_DESC> VertexTypesGetter::g_vertexTypesByElementCount = {
        {VertexPosition::HashId, { VertexPosition::InputLayout} },
        {VertexPositionColor::HashId, {VertexPositionColor::InputLayout} },
        {VertexPositionNormalTexture::HashId, {VertexPositionNormalTexture::InputLayout} }
    };
    
    bool VertexTypesGetter::Get(size_t hashId, D3D12_INPUT_LAYOUT_DESC& outputLayout)
    {
        auto it = g_vertexTypesByElementCount.find(hashId);
        if (it == g_vertexTypesByElementCount.end())
            return false;

        outputLayout = it->second;
        return true;
    }
}