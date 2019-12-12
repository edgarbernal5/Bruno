using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Graphics.Core;

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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetBlendState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetBlendState(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetDepthStencilState(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetRasterizerState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetRasterizerState(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetViewport", CallingConvention = CallingConvention.StdCall)]
        private static extern Viewport Internal_GetViewport(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetBlendState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetBlendState(IntPtr device, IntPtr blendState);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetDepthStencilState(IntPtr device, IntPtr depthStencilState);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetRasterizerState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRasterizerState(IntPtr device, IntPtr rasterizerState);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetViewport", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetViewport(IntPtr device, Viewport viewport);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Present", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Present(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Reset", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Reset(IntPtr device, PresentationParameters parameters);

        bool disposed = false;

        internal IntPtr m_nativePtr;

        public BlendState BlendState
        {
            get
            {
                return new BlendState(Internal_GetBlendState(m_nativePtr));
            }
            set
            {
                Internal_SetBlendState(m_nativePtr, value.m_nativePtr);
            }
        }

        public DepthStencilState DepthStencilState
        {
            get
            {
                return new DepthStencilState(Internal_GetDepthStencilState(m_nativePtr));
            }
            set
            {
                Internal_SetDepthStencilState(m_nativePtr, value.m_nativePtr);
            }
        }

        public RasterizerState RasterizerState
        {
            get
            {
                return new RasterizerState(Internal_GetRasterizerState(m_nativePtr));
            }
            set
            {
                Internal_SetRasterizerState(m_nativePtr, value.m_nativePtr);
            }
        }

        public Viewport Viewport
        {
            get
            {
                return Internal_GetViewport(m_nativePtr);
            }
            set
            {
                Internal_SetViewport(m_nativePtr, value);
            }
        }

        public GraphicsDevice(GraphicsAdapter adapter, PresentationParameters parameters)
        {
            m_nativePtr = Internal_ctor(adapter.m_nativePtr, parameters);

            DepthStencilState = DepthStencilState.Default;
            RasterizerState = RasterizerState.CullCounterClockwise;
            BlendState = BlendState.Opaque;
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
