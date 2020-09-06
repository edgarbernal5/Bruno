
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Core;
using TrioWpfFramework.Net.Graphics.Core;

namespace TrioWpfFramework.Net.Graphics
{
    public class GraphicsDevice : IDisposable
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetDepthStencilState(IntPtr device, IntPtr depthStencilState);

        bool disposed = false;

        internal IntPtr m_nativePtr;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetBlendState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetBlendState(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetBlendState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetBlendState(IntPtr device, IntPtr blendState);

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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetDepthStencilState(IntPtr device);

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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetRasterizerState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetRasterizerState(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetRasterizerState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRasterizerState(IntPtr device, IntPtr rasterizerState);

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


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetViewport", CallingConvention = CallingConvention.StdCall)]
        private static extern Viewport Internal_GetViewport(IntPtr device);
        
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetViewport", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetViewport(IntPtr device, Viewport viewport);

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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor(IntPtr adapter, PresentationParameters parameters);

        public GraphicsDevice(GraphicsAdapter adapter, PresentationParameters parameters)
        {
            m_nativePtr = Internal_ctor(adapter.m_nativePtr, parameters);

            DepthStencilState = DepthStencilState.Default;
            RasterizerState = RasterizerState.CullCounterClockwise;
            BlendState = BlendState.Opaque;
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_dtor(IntPtr device);

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (!disposed)
                {
                    disposed = true;
                    Internal_dtor(m_nativePtr);
                    m_nativePtr = IntPtr.Zero;
                }
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Clear", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Clear(IntPtr device, uint packedColor);

        public void Clear(Color color)
        {
            Internal_Clear(m_nativePtr, color.PackedColor);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Present", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Present(IntPtr device);

        public void Present()
        {
            Internal_Present(m_nativePtr);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Reset", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Reset(IntPtr device, PresentationParameters parameters);

        public void Reset(PresentationParameters parameters)
        {
            Internal_Reset(m_nativePtr, parameters);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetRenderTarget", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRenderTarget(IntPtr device, IntPtr renderTarget);

        public void SetRenderTarget(RenderTarget2D renderTarget)
        {
            if (renderTarget == null)
            {
                Internal_SetRenderTarget(m_nativePtr, IntPtr.Zero);
                return;
            }
            Internal_SetRenderTarget(m_nativePtr, renderTarget.m_nativePtr);
        }
    }
}
