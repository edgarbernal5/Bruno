
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics
{
    public class Model : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Model_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor([MarshalAs(UnmanagedType.LPStr)] string filename);

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

        internal Model(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;

            if (m_nativePointer == IntPtr.Zero) return;

            LoadData();
        }

        public Model(string filename) 
            : base()
        {
            m_nativePointer = Internal_ctor(filename);
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
            IntPtr array = IntPtr.Zero;
            Internal_GetModelMeshes(m_nativePointer, ref array, ref total);

            if (total > 0)
            {
                IntPtr[] parametersPtrs = new IntPtr[total];
                Marshal.Copy(array, parametersPtrs, 0, total);

                var modelMeshes = new ModelMesh[total];
                for (int i = 0; i < total; i++)
                {
                    modelMeshes[i] = new ModelMesh(parametersPtrs[i]);
                }

                Marshal.FreeCoTaskMem(array);
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
