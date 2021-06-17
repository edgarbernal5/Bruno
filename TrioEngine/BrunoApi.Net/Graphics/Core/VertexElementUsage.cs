using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoApi.Net.Graphics.Core
{
    public enum VertexElementUsage : int
    {
        Position,
        Color,
        TextureCoordinate,
        Normal,
        Binormal,
        Tangent,
        BlendIndices,
        BlendWeight,
        Depth,
        Fog,
        PointSize,
        Sample,
        TessellateFactor
    }
}
