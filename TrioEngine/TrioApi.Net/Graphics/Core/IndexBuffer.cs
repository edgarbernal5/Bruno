using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Graphics.Core
{
    public class IndexBuffer
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "IndexBuffer_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device, int elementSize, int indexCount, int usage);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "IndexBuffer_SetData", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetData(IntPtr buffer, IntPtr bytes, uint elementCount, uint sizeArray);


        internal IntPtr m_nativePtr;

        public IndexBuffer(GraphicsDevice device, IndexElementSize elementSize, int indexCount, ResourceUsage usage)
        {
            m_nativePtr = Internal_Ctor(device.m_nativePtr, (int)elementSize, indexCount, (int)usage);
        }

        internal IndexBuffer(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
        }

        public void SetData<T>(T[] data) where T : struct
        {
            SetDataInternal(0, data, 0, data.Length, SetDataOptions.None);
        }

        private void SetDataInternal<T>(int offsetInBytes, T[] data, int startIndex, int elementCount, SetDataOptions options) where T : struct
        {
            int elementSize = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            try
            {
                var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                Internal_SetData(m_nativePtr, dataPtr, (uint)(data.Length), (uint)(data.Length * elementSize));
            }
            finally
            {
                dataHandle.Free();
            }
        }
    }
}
