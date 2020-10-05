
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Utilities;

namespace TrioApi.Net.Graphics.Core
{
    public class Texture2D : Texture
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_TestLoadFromFile", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TestLoadFromFile(IntPtr texture);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetWidth", CallingConvention = CallingConvention.StdCall)]
        private static extern int Internal_GetWidth(IntPtr texture);
        public int Width
        {
            get
            {
                return Internal_GetWidth(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetHeight", CallingConvention = CallingConvention.StdCall)]
        private static extern int Internal_GetHeight(IntPtr texture);
        public int Height
        {
            get
            {
                return Internal_GetHeight(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device, int width, int height);

        public Texture2D(GraphicsDevice device, int width, int height)
        {
            m_nativePointer = Internal_Ctor(device.NativePointer, width, height);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor2(IntPtr device, int width, int height, int format);

        public Texture2D(GraphicsDevice device, int width, int height, SurfaceFormat format)
        {
            m_nativePointer = Internal_Ctor2(device.NativePointer, width, height, (int)format);
        }

        internal Texture2D(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        internal Texture2D()
        {
            m_nativePointer = IntPtr.Zero;
        }

        public void LoadFromFile()
        {
            Internal_TestLoadFromFile(m_nativePointer);
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetData", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetData(IntPtr texture, IntPtr bytes, uint elementCount);
        public void GetData<T>(T[] data) where T : struct
        {
            int elementSizeInBytes = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            try
            {
                var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                int baseElementInBytes = Format.GetByteSize();
                uint elementCount = (uint)(elementSizeInBytes * data.Length / baseElementInBytes);
                Internal_GetData(m_nativePointer, dataPtr, elementCount);
            }
            finally
            {
                dataHandle.Free();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetData_A", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetData_A(IntPtr texture, IntPtr bytes, uint elementCount);

        public void GetData<T>(int level, int arraySlice, Rectangle? rect, T[] data, int startIndex, int elementCount) where T : struct
        {
            int elementSize = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            try
            {
                //TODO:
                //var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                //int baseElementInBytes = Format.GetByteSize();
                //uint elemCount = (uint)(elementSize * elementCount / baseElementInBytes);
                //Internal_GetData_A(m_nativePtr, dataPtr, elemCount);
            }
            finally
            {
                dataHandle.Free();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_Dctor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Dctor(IntPtr texture);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                // TODO: dispose managed state (managed objects).
            }

            // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
            // TODO: set large fields to null.
            Internal_Dctor(m_nativePointer);
        }
    }
}
