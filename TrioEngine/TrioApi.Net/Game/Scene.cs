
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Game
{
    public class Scene : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_GetActiveScene", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetActiveScene();

        public static Scene ActiveScene
        {
            get
            {
                return new Scene(Internal_GetActiveScene());
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        public Scene() 
            : base()
        {
            m_nativePointer = Internal_ctor();
        }

        internal Scene(IntPtr nativePtr) 
            : base(nativePtr)
        {
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_Dtor", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Dtor(IntPtr scene);

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
            }
            Internal_Dtor(NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_Update", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Update();

        public void Update()
        {
            Internal_Update();
        }
    }
}
