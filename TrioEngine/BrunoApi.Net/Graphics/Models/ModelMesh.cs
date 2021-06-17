using Bruno.Collections;
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics
{
    public class ModelMesh : CppObject, INamedItem
    {

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMesh_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr modelMesh);

        public string Name
        {
            get
            {
                return Internal_GetName(m_nativePointer);
            }
        }

        public ModelMeshPart[] ModelMeshParts
        {
            get
            {
                return m_modelMeshParts;
            }
        }
        private ModelMeshPart[] m_modelMeshParts;


        internal ModelMesh(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
            LoadData();
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ModelMesh_GetModelMeshParts", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetModelMeshParts(IntPtr modelMesh, ref IntPtr modelMeshParts, ref int size);

        private void LoadData()
        {
            int total = 0;
            IntPtr array = IntPtr.Zero;
            Internal_GetModelMeshParts(m_nativePointer, ref array, ref total);

            if (total > 0)
            {
                IntPtr[] parametersPtrs = new IntPtr[total];
                Marshal.Copy(array, parametersPtrs, 0, total);

                var modelMeshParts = new ModelMeshPart[total];
                for (int i = 0; i < total; i++)
                {
                    modelMeshParts[i] = new ModelMeshPart(parametersPtrs[i]);
                }

                Marshal.FreeCoTaskMem(array);
                m_modelMeshParts = modelMeshParts;
            }
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

