
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics
{
    public class Model : CppObject
    {
        //lista de mesh

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor([MarshalAs(UnmanagedType.LPStr)] string filename);

        public Model():base()
        {

        }

        internal Model(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        public Model(string filename) 
            : base()
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
