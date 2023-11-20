#pragma once

#include "Bruno/Math/Math.h"
#include <unordered_map>
#include <vector>

namespace Bruno
{
    struct VertexPosition
    {
        VertexPosition() = default;

        explicit VertexPosition(const Math::Vector3& position)
            : Position(position)
        {}

        explicit VertexPosition(DirectX::FXMVECTOR position)
        {
            DirectX::XMStoreFloat3(&(this->Position), position);
        }

        Math::Vector3 Position;

        static const D3D12_INPUT_LAYOUT_DESC& InputLayout;
        static const size_t HashId;
    private:
        static const int                      InputElementCount = 1;
        static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
    };

    struct VertexPositionColor
    {
        VertexPositionColor() = default;

        explicit VertexPositionColor(const Math::Vector3& position, 
            const Math::Vector3& color)
            : Position(position),
              Color(color)
        {}

        explicit VertexPositionColor(DirectX::FXMVECTOR position, DirectX::FXMVECTOR color)
        {
            DirectX::XMStoreFloat3(&(this->Position), position);
            DirectX::XMStoreFloat3(&(this->Color), color);
        }

        Math::Vector3 Position;
        Math::Vector3 Color;

        static const D3D12_INPUT_LAYOUT_DESC& InputLayout;
        static const size_t HashId;
    private:
        static const int                      InputElementCount = 2;
        static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
    };

    struct VertexPositionNormalTexture
    {
        VertexPositionNormalTexture() = default;

        explicit VertexPositionNormalTexture(const Math::Vector3& position,
            const Math::Vector3& normal, const Math::Vector2& texture)
            :   Position(position),
            Normal(normal),
                Texture(texture)
        {}

        explicit VertexPositionNormalTexture(DirectX::FXMVECTOR position, DirectX::FXMVECTOR normal, DirectX::FXMVECTOR texture)
        {
            DirectX::XMStoreFloat3(&(this->Position), position);
            DirectX::XMStoreFloat3(&(this->Normal), normal);
            DirectX::XMStoreFloat2(&(this->Texture), texture);
        }

        Math::Vector3 Position;
        Math::Vector3 Normal;
        Math::Vector2 Texture;

        static const D3D12_INPUT_LAYOUT_DESC& InputLayout;
        static const size_t HashId;
    private:
        static const int                      InputElementCount = 3;
        static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];
    };

    class VertexTypesGetter
    {
    public:
        static bool Get(size_t hashId, D3D12_INPUT_LAYOUT_DESC& outputLayout);

    private:
        static std::unordered_map<size_t, const D3D12_INPUT_LAYOUT_DESC> g_vertexTypesByElementCount;
    };
}