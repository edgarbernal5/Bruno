
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics
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
