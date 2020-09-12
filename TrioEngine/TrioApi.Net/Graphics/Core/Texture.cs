using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net;
using TrioWpfFramework.Net.Graphics;

namespace TrioApi.Net.Graphics.Core
{
    public abstract class Texture : IDisposable
    {
        internal IntPtr m_nativePtr;

        protected bool m_disposed;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture_GetFormat", CallingConvention = CallingConvention.StdCall)]
        private static extern SurfaceFormat Internal_GetFormat(IntPtr texture);

        public SurfaceFormat Format
        {
            get
            {
                return Internal_GetFormat(m_nativePtr);
            }
        }

        protected Texture()
        {
            m_nativePtr = IntPtr.Zero;
            m_disposed = false;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected abstract void Dispose(bool disposing);
    }
}
