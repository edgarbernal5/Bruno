using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics
{
    public enum RenderTargetUsage
    {
        // The render target content will not be preserved.
        DiscardContents,

        // The render target content will be preserved even if it is slow or requires extra memory.
        PreserveContents,

        // The render target content might be preserved if the platform can do so without a penalty in performance or memory usage.
        PlatformContents
    }
}
