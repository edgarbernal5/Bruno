using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics
{
    public class GraphicsDevice : IDisposable
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor(IntPtr adapter, PresentationParameters parameters);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor2(IntPtr adapter, int height, int width, int surfaceFormat, int depthFormat, bool isfullScreen, IntPtr handle);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_dtor(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Clear", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Clear(IntPtr device, uint packedColor);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Present", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Present(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Reset", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Reset(IntPtr device, PresentationParameters parameters);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetViewport", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetViewport(IntPtr device, Viewport viewport);

        bool disposed = false;

        internal IntPtr m_nativePtr;

        public Viewport Viewport
        {
            get
            {
                Viewport x=default(Viewport);
                return x;
            }
            set
            {
                Internal_SetViewport(m_nativePtr, value);
            }
        }

        public GraphicsDevice(GraphicsAdapter adapter, PresentationParameters parameters)
        {
            m_nativePtr = Internal_ctor(adapter.m_nativePtr, parameters);
        }

        public void Present()
        {
            Internal_Present(m_nativePtr);
        }

        public void Reset(PresentationParameters parameters)
        {
            Internal_Reset(m_nativePtr, parameters);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (!disposed)
                {
                    disposed = true;
                    Internal_dtor(m_nativePtr);
                }
            }
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}
