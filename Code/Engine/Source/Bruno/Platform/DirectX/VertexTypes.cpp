#include "brpch.h"
#include "VertexTypes.h"

namespace Bruno
{
    std::vector<InputElementDesc> VertexPosition::GetLayout()
    {
        return
        {
            { "POSITION", 0, VertexFormat::Float3 }
        };
    }

    std::vector<InputElementDesc> VertexPositionNormalTexture::GetLayout()
    {
        return
        {
            { "POSITION", 0, VertexFormat::Float3 },
            { "NORMAL",   0, VertexFormat::Float3 },
            { "TEXCOORD", 0, VertexFormat::Float2 }
        };
    }
    
    std::vector<InputElementDesc> VertexPositionColor::GetLayout()
    {
        return
        {
            { "POSITION", 0, VertexFormat::Float3 },
            { "COLOR",   0, VertexFormat::Float4 }
        };
    }
}
