
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics.Models
{
    public class Mesh : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor([MarshalAs(UnmanagedType.LPStr)] string filename);

        public Mesh(string filename) : base()
        {
            m_nativePointer = Internal_ctor(filename);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_dtor(IntPtr model);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
            Internal_dtor(m_nativePointer);
        }
    }
}
