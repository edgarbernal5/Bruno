using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoApi.Net.Graphics
{
    public enum DepthFormat
    {
        // Depth-stencil buffer will not be created.
        None,

        // 16-bit depth buffer.
        Depth16,

        // 24-bit depth buffer. Equivalent of <see cref="DepthFormat.Depth24Stencil8"/> for DirectX platforms.
        Depth24,

        // 32-bit depth-stencil buffer. Where 24-bit depth and 8-bit for stencil used.
        Depth24Stencil8,

        // 32-bit
        Depth32
    }
}
