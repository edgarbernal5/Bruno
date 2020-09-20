
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics.Core
{
    public class RenderTarget2D : Texture2D
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderTarget2D_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device, int width, int height, int preferredFormat);
        
        public RenderTarget2D(GraphicsDevice device, int width, int height)
        {
            m_nativePtr = Internal_Ctor(device.NativePointer, width, height, (int)SurfaceFormat.Color);
        }

        public RenderTarget2D(GraphicsDevice device, int width, int height, SurfaceFormat preferredFormat)
        {
            m_nativePtr = Internal_Ctor(device.NativePointer, width, height, (int)preferredFormat);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderTarget2D_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor2(IntPtr device, int width, int height, uint mipmap, int preferredFormat, int preferredDepthFormat, int renderTargetUsage);

        public RenderTarget2D(GraphicsDevice device, int width, int height, uint mipmap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat, RenderTargetUsage usage)
        {
            m_nativePtr = Internal_Ctor2(device.NativePointer, width, height, mipmap, (int)preferredFormat, (int)preferredDepthFormat, (int)usage);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderTarget2D_Ctor3", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor3(IntPtr device, int width, int height, uint mipmap, int preferredFormat, int preferredDepthFormat);

        public RenderTarget2D(GraphicsDevice device, int width, int height, uint mipmap, SurfaceFormat preferredFormat, DepthFormat preferredDepthFormat)
        {
            m_nativePtr = Internal_Ctor3(device.NativePointer, width, height, mipmap, (int)preferredFormat, (int)preferredDepthFormat);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "RenderTarget2D_Dctor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Dctor(IntPtr renderTarget);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                // TODO: dispose managed state (managed objects).
            }

            // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
            // TODO: set large fields to null.
            Internal_Dctor(m_nativePtr);
        }

    }
}
