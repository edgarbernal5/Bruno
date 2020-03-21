using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PresentationParameters
    {
        public SurfaceFormat BackBufferFormat;
        public DepthFormat DepthStencilFormat;

        public int BackBufferHeight, BackBufferWidth;
        public int MultiSampleCount;

        public bool IsFullScreen;

        public IntPtr DeviceWindowHandle;

        public PresentInterval PresentationInterval;
        public RenderTargetUsage Usage;
    }
}
