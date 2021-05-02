using Estero.Interop;
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics.Core;

namespace TrioApi.Net.Graphics
{
    public class ModelMeshPart : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetVertexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetVertexBuffer(IntPtr modelMeshPart);

        public VertexBuffer VertexBuffer
        {
            get
            {
                return new VertexBuffer(Internal_GetVertexBuffer(m_nativePointer));
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetIndexBuffer", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetIndexBuffer(IntPtr modelMeshPart);

        public IndexBuffer IndexBuffer
        {
            get
            {
                return new IndexBuffer(Internal_GetIndexBuffer(m_nativePointer));
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetMaterial", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetMaterial(IntPtr modelMeshPart);

        public Material Material
        {
            get
            {
                return new Material(Internal_GetMaterial(m_nativePointer));
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetVertexCount", CallingConvention = CallingConvention.StdCall)]
        private static extern uint Internal_GetVertexCount(IntPtr modelMeshPart);

        public uint VertexCount
        {
            get
            {
                return Internal_GetVertexCount(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetPrimitiveCount", CallingConvention = CallingConvention.StdCall)]
        private static extern uint Internal_GetPrimitiveCount(IntPtr modelMeshPart);

        public uint PrimitiveCount
        {
            get
            {
                return Internal_GetPrimitiveCount(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetVertexOffset", CallingConvention = CallingConvention.StdCall)]
        private static extern uint Internal_GetVertexOffset(IntPtr modelMeshPart);

        public uint VertexOffset
        {
            get
            {
                return Internal_GetVertexOffset(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetStartIndex", CallingConvention = CallingConvention.StdCall)]
        private static extern uint ModelMeshPart_GetStartIndex(IntPtr modelMeshPart);

        public uint StartIndex
        {
            get
            {
                return ModelMeshPart_GetStartIndex(m_nativePointer);
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMeshPart_GetPrimitiveType", CallingConvention = CallingConvention.StdCall)]
        private static extern PrimitiveType ModelMeshPart_GetPrimitiveType(IntPtr modelMeshPart);

        public PrimitiveType PrimitiveType
        {
            get
            {
                return ModelMeshPart_GetPrimitiveType(m_nativePointer);
            }
        }

        internal ModelMeshPart(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMesh_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_dtor(IntPtr modelMesh);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
            Internal_dtor(m_nativePointer);
        }
    }
}

