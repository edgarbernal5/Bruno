﻿
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;
using BrunoApi.Net.Graphics.Core;

namespace BrunoApi.Net.Graphics
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
                return new BlendState(Internal_GetBlendState(m_nativePointer));
            }
            set
            {
                Internal_SetBlendState(m_nativePointer, value.m_nativePtr);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetDepthStencilState", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetDepthStencilState(IntPtr device);

        public DepthStencilState DepthStencilState
        {
            get
            {
                return new DepthStencilState(Internal_GetDepthStencilState(m_nativePointer));
            }
            set
            {
                Internal_SetDepthStencilState(m_nativePointer, value.m_nativePtr);
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
                return new RasterizerState(Internal_GetRasterizerState(m_nativePointer));
            }
            set
            {
                Internal_SetRasterizerState(m_nativePointer, value.m_nativePtr);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_GetPresentationParameters", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetPresentationParameters(IntPtr device, ref PresentationParameters parameters);

        public PresentationParameters PresentationParameters
        {
            get
            {
                PresentationParameters result = new PresentationParameters();
                Internal_GetPresentationParameters(m_nativePointer, ref result);

                return result;
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
                return Internal_GetViewport(m_nativePointer);
            }
            set
            {
                Internal_SetViewport(m_nativePointer, value);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor(IntPtr adapter, PresentationParameters parameters);

        public GraphicsDevice(GraphicsAdapter adapter, PresentationParameters parameters)
        {
            m_nativePointer = Internal_ctor(adapter.NativePointer, parameters);

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
            Internal_dtor(m_nativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Clear", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Clear(IntPtr device, ref float colorAsFloat);

        public void Clear(Color color)
        {
            Internal_Clear(m_nativePointer, ref color.R);
        }
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_ClearAsRGBA8", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_ClearRGBA8(IntPtr device, uint packedColor);

        public void Clear(ColorRGBA8 color)
        {
            Internal_ClearRGBA8(m_nativePointer, color.PackedColor);
        }

        public void DrawUserPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int primitiveCount) where T : struct, IVertexType
        {
            DrawUserPrimitives<T>(primitiveType, vertexData, vertexOffset, primitiveCount, VertexDeclarationFactory<T>.VertexDeclaration);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_DrawUserPrimitives", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawUserPrimitives(IntPtr device, PrimitiveType primitiveType, IntPtr bytes, int length, int vertexOffset, uint primitiveCount, IntPtr vertexDeclaration);

        public void DrawUserPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct
        {
            int elementSize = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(vertexData, GCHandleType.Pinned);

            try
            {
                var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                Internal_DrawUserPrimitives(m_nativePointer, primitiveType, dataPtr, vertexData.Length, vertexOffset, (uint)(primitiveCount), vertexDeclaration.NativePointer);
            }
            finally
            {
                dataHandle.Free();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_DrawUserIndexedPrimitives", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawUserIndexedPrimitives(IntPtr device, PrimitiveType primitiveType, IntPtr vertexData, int vertexSize, int vertexOffset, int numVertices, IntPtr indexData, int indexLength, IndexElementSize indexElementSize, int indexOffset, int primitiveCount, IntPtr vertexDeclaration);

        public void DrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, short[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct
        {
            var vertexDataHandle = GCHandle.Alloc(vertexData, GCHandleType.Pinned);
            var indexDataHandle = GCHandle.Alloc(indexData, GCHandleType.Pinned);

            try
            {
                var vertexDataPtr = (IntPtr)(vertexDataHandle.AddrOfPinnedObject().ToInt64());
                var indexDataPtr = (IntPtr)(indexDataHandle.AddrOfPinnedObject().ToInt64());

                Internal_DrawUserIndexedPrimitives(m_nativePointer, primitiveType, vertexDataPtr, vertexData.Length, vertexOffset, numVertices, 
                    indexDataPtr, indexData.Length, IndexElementSize.SixteenBits, indexOffset, primitiveCount, vertexDeclaration.NativePointer);
            }
            finally
            {
                vertexDataHandle.Free();
                indexDataHandle.Free();
            }
        }

        public void DrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, int[] indexData, int indexOffset, int primitiveCount, VertexDeclaration vertexDeclaration) where T : struct
        {
            var vertexDataHandle = GCHandle.Alloc(vertexData, GCHandleType.Pinned);
            var indexDataHandle = GCHandle.Alloc(indexData, GCHandleType.Pinned);

            try
            {
                var vertexDataPtr = (IntPtr)(vertexDataHandle.AddrOfPinnedObject().ToInt64());
                var indexDataPtr = (IntPtr)(indexDataHandle.AddrOfPinnedObject().ToInt64());

                Internal_DrawUserIndexedPrimitives(m_nativePointer, primitiveType, vertexDataPtr, vertexData.Length, vertexOffset, numVertices,
                    indexDataPtr, indexData.Length, IndexElementSize.ThirtyTwoBits, indexOffset, primitiveCount, vertexDeclaration.NativePointer);
            }
            finally
            {
                vertexDataHandle.Free();
                indexDataHandle.Free();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_DrawIndexedPrimitives", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawIndexedPrimitives(IntPtr device, PrimitiveType primitiveType, uint baseVertex, uint startIndex, uint primitiveCount);

        public void DrawIndexedPrimitives(PrimitiveType primitiveType, uint baseVertex, uint startIndex, uint primitiveCount)
        {
            Internal_DrawIndexedPrimitives(m_nativePointer, primitiveType, baseVertex, startIndex, primitiveCount);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_DrawIndexedPrimitives", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_DrawUserIndexedPrimitives(IntPtr device, PrimitiveType primitiveType, uint baseVertex, uint startIndex, uint primitiveCount);

        public void DrawUserIndexedPrimitives<T>(PrimitiveType primitiveType, T[] vertexData, int vertexOffset, int numVertices, int[] indexData, int indexOffset, int primitiveCount) where T : struct, IVertexType
        {

        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Flush", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Flush(IntPtr device);

        public void Flush()
        {
            Internal_Flush(m_nativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Present", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Present(IntPtr device);

        public void Present()
        {
            Internal_Present(m_nativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_PrepareRenderWindow", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_PrepareRenderWindow(IntPtr device, IntPtr hwnd);

        public void PrepareRenderWindow(IntPtr hwnd)
        {
            Internal_PrepareRenderWindow(m_nativePointer, hwnd);
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_PresentHWND", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_PresentHWND(IntPtr device, IntPtr hwndHost);

        public void Present(IntPtr hwndHost)
        {
            Internal_PresentHWND(m_nativePointer, hwndHost);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_RemoveHwnd", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_RemoveHwnd(IntPtr device, IntPtr hwnd);

        public void RemoveHwnd(IntPtr hwnd)
        {
            Internal_RemoveHwnd(m_nativePointer, hwnd);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_Reset", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Reset(IntPtr device, PresentationParameters parameters);

        public void Reset(PresentationParameters parameters)
        {
            Internal_Reset(m_nativePointer, parameters);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetRenderTarget", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetRenderTarget(IntPtr device, IntPtr renderTarget);

        public void SetRenderTarget(RenderTarget2D renderTarget)
        {
            if (renderTarget == null)
            {
                Internal_SetRenderTarget(m_nativePointer, IntPtr.Zero);
                return;
            }
            Internal_SetRenderTarget(m_nativePointer, renderTarget.NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetVertexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetVertexBuffer(IntPtr device, IntPtr vertexBuffer);

        public void SetVertexBuffer(VertexBuffer vertexBuffer)
        {
            if (vertexBuffer == null)
            {
                Internal_SetVertexBuffer(m_nativePointer, IntPtr.Zero);
                return;
            }
            Internal_SetVertexBuffer(m_nativePointer, vertexBuffer.NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsDevice_SetIndexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetIndexBuffer(IntPtr device, IntPtr indexBuffer);

        public void SetIndexBuffer(IndexBuffer indexBuffer)
        {
            if (indexBuffer == null)
            {
                Internal_SetIndexBuffer(m_nativePointer, IntPtr.Zero);
                return;
            }
            Internal_SetIndexBuffer(m_nativePointer, indexBuffer.NativePointer);
        }
    }
}
