
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Game
{
    public class Scene
    {
        internal IntPtr m_nativePtr;

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
        {
            m_nativePtr = Internal_ctor();
        }

        internal Scene(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
        }
    }
}
