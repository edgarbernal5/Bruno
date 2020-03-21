using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class VertexDeclaration
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr _ctor(int vertexStride, VertexElement[] elements, int sizeElements);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "VertexDeclaration_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr _ctor(VertexElement[] elements, int sizeElements);

        internal IntPtr m_nativePtr;

        public VertexDeclaration(int vertexStride, VertexElement[] elements)
        {
            m_nativePtr = _ctor(vertexStride, elements, elements.Length);
        }

        public VertexDeclaration(VertexElement[] elements)
        {
            m_nativePtr = _ctor(elements, elements.Length);
        }

        internal VertexDeclaration(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
        }
    }
}
