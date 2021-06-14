
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics.Core
{
    public class VertexBuffer : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexBuffer_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device, IntPtr vertexDeclaration, int vertexCount, int usage);

        public VertexBuffer(GraphicsDevice device, VertexDeclaration vertexDeclaration, int vertexCount, ResourceUsage usage)
        {
            m_nativePointer = Internal_Ctor(device.NativePointer, vertexDeclaration.NativePointer, vertexCount, (int)usage);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexBuffer_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor2(IntPtr device, IntPtr vertexDeclaration, int vertexCount);

        public VertexBuffer(GraphicsDevice device, VertexDeclaration vertexDeclaration, int vertexCount)
        {
            m_nativePointer = Internal_Ctor2(device.NativePointer, vertexDeclaration.NativePointer, vertexCount);
        }

        internal VertexBuffer(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        public void SetData<T>(T[] data) where T : struct
        {
            SetDataInternal(0, data, 0, data.Length, SetDataOptions.None);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexBuffer_SetData", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetData(IntPtr buffer, IntPtr bytes, uint elementCount, uint sizeArray);

        private void SetDataInternal<T>(int offsetInBytes, T[] data, int startIndex, int elementCount, SetDataOptions options) where T : struct
        {
            int elementSize = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            try
            {
                var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                Internal_SetData(m_nativePointer, dataPtr, (uint)(data.Length), (uint)(data.Length * elementSize));
            }
            finally
            {
                dataHandle.Free();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexBuffer_Dctor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Dctor(IntPtr buffer);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                Internal_Dctor(m_nativePointer);
            }
        }
    }
}
