
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
{
    public class VertexDeclaration : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr _ctor(int vertexStride, VertexElement[] elements, int sizeElements);

        public VertexDeclaration(int vertexStride, VertexElement[] elements)
        {
            m_nativePointer = _ctor(vertexStride, elements, elements.Length);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr _ctor(VertexElement[] elements, int sizeElements);

        public VertexDeclaration(VertexElement[] elements)
        {
            m_nativePointer = _ctor(elements, elements.Length);
        }

        internal VertexDeclaration(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        internal static VertexDeclaration FromType(Type vertexType)
        {
            IVertexType vertexType2 = Activator.CreateInstance(vertexType) as IVertexType;
            VertexDeclaration vertexDeclaration = vertexType2.VertexDeclaration;
            return vertexDeclaration;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_Dctor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Dctor(IntPtr vertexDeclaration);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                //Internal_Dctor(m_nativePointer);
            }
        }
    }
}
