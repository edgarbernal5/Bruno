
using Estero.Interop;
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Core;

namespace TrioApi.Net.Graphics
{
    public class GraphicsDevice : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetDepthStencilState(IntPtr device, IntPtr depthStencilState);

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
            m_nativePtr = Internal_ctor(adapter.NativePointer, parameters);

            DepthStencilState = DepthStencilState.Default;
            RasterizerState = RasterizerState.CullCounterClockwise;
            BlendState = BlendState.Opaque;
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_dtor(IntPtr device);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
            }
            Internal_dtor(m_nativePtr);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Clear", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Clear(IntPtr device, ref float colorAsFloat);

        public void Clear(Color color)
        {
            Internal_Clear(m_nativePtr, ref color.R);
        }
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_ClearAsRGBA8", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_ClearRGBA8(IntPtr device, uint packedColor);

        public void Clear(ColorRGBA8 color)
        {
            Internal_ClearRGBA8(m_nativePtr, color.PackedColor);
        }
        

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_DrawIndexedPrimitives", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawIndexedPrimitives(IntPtr device, PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount);

        public void DrawIndexedPrimitives(PrimitiveType primitiveType, int baseVertex, int minVertexIndex, int numVertices, int startIndex, int primitiveCount)
        {
            Internal_DrawIndexedPrimitives(m_nativePtr, primitiveType, baseVertex, minVertexIndex, numVertices, startIndex, primitiveCount);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Flush", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Flush(IntPtr device);

        public void Flush()
        {
            Internal_Flush(m_nativePtr);
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
            Internal_SetRenderTarget(m_nativePtr, renderTarget.NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetVertexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetVertexBuffer(IntPtr device, IntPtr vertexBuffer);

        public void SetVertexBuffer(VertexBuffer vertexBuffer)
        {
            if (vertexBuffer == null)
            {
                Internal_SetVertexBuffer(m_nativePtr, IntPtr.Zero);
                return;
            }
            Internal_SetVertexBuffer(m_nativePtr, vertexBuffer.NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetIndexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetIndexBuffer(IntPtr device, IntPtr indexBuffer);

        public void SetIndexBuffer(IndexBuffer indexBuffer)
        {
            if (indexBuffer == null)
            {
                Internal_SetIndexBuffer(m_nativePtr, IntPtr.Zero);
                return;
            }
            Internal_SetIndexBuffer(m_nativePtr, indexBuffer.NativePointer);
        }
    }
}
