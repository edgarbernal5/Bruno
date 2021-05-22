
using Estero.Interop;
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Core;

namespace TrioApi.Net.Graphics
{
    public class Material : CppObject
    {
        internal Material(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Material_Ctor", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_ctor([MarshalAs(UnmanagedType.LPStr)] string name);

        public Material(string name)
        {
            m_nativePointer = Internal_ctor(name);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Material_InsertTexture", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void Internal_InsertTexture(IntPtr material, [MarshalAs(UnmanagedType.LPStr)] string name, IntPtr texture);


        public void InsertTexture(string name, Texture texture)
        {
            Internal_InsertTexture(m_nativePointer, name, texture.NativePointer);
        }

        protected override void OnDisposing(bool disposing)
        {

        }
    }
}
