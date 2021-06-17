
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;
using BrunoApi.Net.Graphics.Core;

namespace BrunoApi.Net.Graphics
{
    public class Model : CppObject
    {
        public ModelMeshCollection ModelMeshes
        {
            get
            {
                return m_modelMeshes;
            }
        }
        private ModelMeshCollection m_modelMeshes;


        public Model()
            : base()
        {
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor(IntPtr device);

        public Model(GraphicsDevice device)
            : base()
        {
            m_nativePointer = Internal_ctor(device.NativePointer);
        }

        internal Model(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;

            if (m_nativePointer == IntPtr.Zero) return;

            LoadData();
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Ctor2", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor(IntPtr device, IntPtr vertexBuffer, IntPtr indexBuffer, IntPtr material);

        public Model(GraphicsDevice device, VertexBuffer vertexBuffer, IndexBuffer indexBuffer, Material material) 
            : base()
        {
            m_nativePointer = Internal_ctor(device.NativePointer, vertexBuffer.NativePointer, indexBuffer.NativePointer, material.NativePointer);
        }

        private void LoadData()
        {
            LoadModelMeshes();
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_GetModelMeshes", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetModelMeshes(IntPtr model, ref IntPtr modelMeshes, ref int size);

        private void LoadModelMeshes()
        {
            int total = 0;
            IntPtr unmanagedArray = IntPtr.Zero;
            Internal_GetModelMeshes(m_nativePointer, ref unmanagedArray, ref total);

            if (total > 0)
            {
                IntPtr[] unmanagedModelMeshes = new IntPtr[total];
                Marshal.Copy(unmanagedArray, unmanagedModelMeshes, 0, total);

                var modelMeshes = new ModelMesh[total];
                for (int i = 0; i < total; i++)
                {
                    modelMeshes[i] = new ModelMesh(unmanagedModelMeshes[i]);
                }

                Marshal.FreeCoTaskMem(unmanagedArray);
                m_modelMeshes = new ModelMeshCollection(modelMeshes);
            }
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
