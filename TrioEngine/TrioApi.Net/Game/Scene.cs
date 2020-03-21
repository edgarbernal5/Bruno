using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Game
{
    public class Scene
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_ctor();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_GetActiveScene", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetActiveScene();

        internal IntPtr m_nativePtr;

        public static Scene ActiveScene
        {
            get
            {
                return new Scene(Internal_GetActiveScene());
            }
        }

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
