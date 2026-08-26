#pragma once

#include "Bruno/Math/Math.h"
#include <unordered_map>
#include <vector>

#include "Bruno/Renderer/RHITypes.h"

namespace Bruno
{
    struct VertexPositionNormalTexture
    {
        Math::Vector3 Position;
        Math::Vector3 Normal;
        Math::Vector2 TexCoord;

        static std::vector<InputElementDesc> GetLayout();
    };
    
    struct VertexPositionColor
    {
        Math::Vector3 Position;
        Math::Vector4 Color;
        
        static std::vector<InputElementDesc> GetLayout();
    };
}
