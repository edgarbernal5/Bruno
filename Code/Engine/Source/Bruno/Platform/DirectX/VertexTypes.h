#pragma once

#include "D3DCommon.h"

namespace Bruno
{
    struct VertexPosition
    {
        VertexPosition() = default;

        explicit VertexPosition(const DirectX::XMFLOAT3& position)
            : Position(position)
        {}

        explicit VertexPosition(DirectX::FXMVECTOR position)
        {
            DirectX::XMStoreFloat3(&(this->Position), position);
        }

        DirectX::XMFLOAT3 Position;

        static const D3D12_INPUT_LAYOUT_DESC InputLayout;
    private:
        static const int                      InputElementCount = 1;
        static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
    };

    struct VertexPositionColor
    {
        VertexPositionColor() = default;

        explicit VertexPositionColor(const DirectX::XMFLOAT3& position, 
            const DirectX::XMFLOAT3& color)
            : Position(position),
              Color(color)
        {}

        explicit VertexPositionColor(DirectX::FXMVECTOR position, DirectX::FXMVECTOR color)
        {
            DirectX::XMStoreFloat3(&(this->Position), position);
            DirectX::XMStoreFloat3(&(this->Color), color);
        }

        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Color;

        static const D3D12_INPUT_LAYOUT_DESC InputLayout;
    private:
        static const int                      InputElementCount = 2;
        static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
    };
}