
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
{
    public abstract class Texture : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Texture_GetFormat", CallingConvention = CallingConvention.StdCall)]
        private static extern SurfaceFormat Internal_GetFormat(IntPtr texture);

        public SurfaceFormat Format
        {
            get
            {
                return Internal_GetFormat(m_nativePointer);
            }
        }

        protected Texture() 
            : base()
        {
            m_nativePointer = IntPtr.Zero;
        }
    }
}
