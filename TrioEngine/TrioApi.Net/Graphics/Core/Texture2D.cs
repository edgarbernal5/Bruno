using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net;
using TrioWpfFramework.Net.Graphics;
using TrioWpfFramework.Net.Graphics.Core;

namespace TrioApi.Net.Graphics.Core
{
    public class Texture2D : IDisposable
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device, int width, int height);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor2(IntPtr device, int width, int height, int format);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetData", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetData(IntPtr texture, IntPtr bytes, uint elementCount, uint sizeArray);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_TestLoadFromFile", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_TestLoadFromFile(IntPtr texture);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetWidth", CallingConvention = CallingConvention.StdCall)]
        private static extern int Internal_GetWidth(IntPtr texture);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture2D_GetHeight", CallingConvention = CallingConvention.StdCall)]
        private static extern int Internal_GetHeight(IntPtr texture);

        internal IntPtr m_nativePtr;

        protected bool m_disposed = false;

        public int Width { get { return Internal_GetWidth(m_nativePtr); } }

        public int Height { get { return Internal_GetHeight(m_nativePtr); } }

        public Texture2D(GraphicsDevice device, int width, int height)
        {
            m_nativePtr = Internal_Ctor(device.m_nativePtr, width, height);
        }

        public Texture2D(GraphicsDevice device, int width, int height, SurfaceFormat format)
        {
            m_nativePtr = Internal_Ctor2(device.m_nativePtr, width, height, (int)format);
        }

        internal Texture2D(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
        }

        internal Texture2D()
        {
            m_nativePtr = IntPtr.Zero;
        }

        public void LoadFromFile()
        {
            Internal_TestLoadFromFile(m_nativePtr);
        }

        public void GetData<T>(T[] data) where T : struct
        {
            GetData(0, 0, null, data, 0, data.Length);
        }

        public void GetData<T>(int level, int arraySlice, Rectangle? rect, T[] data, int startIndex, int elementCount) where T : struct
        {
            int elementSize = Marshal.SizeOf(typeof(T));
            var dataHandle = GCHandle.Alloc(data, GCHandleType.Pinned);

            try
            {
                var dataPtr = (IntPtr)(dataHandle.AddrOfPinnedObject().ToInt64());

                Internal_GetData(m_nativePtr, dataPtr, (uint)(data.Length), (uint)(data.Length * elementSize));
            }
            finally
            {
                dataHandle.Free();
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
        }
    }
}
