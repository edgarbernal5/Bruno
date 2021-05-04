
using Estero.Interop;
using System;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics;
using static TrioApi.Net.Renderer.Renderer;

namespace TrioApi.Net.Game
{
    public class Scene : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_GetActiveScene", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetActiveScene();

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_SetActiveScene", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetActiveScene(IntPtr scene);

        public static Scene ActiveScene
        {
            get
            {
                return new Scene(Internal_GetActiveScene());
            }
            set
            {
                if (value == null)
                {
                    Internal_SetActiveScene(IntPtr.Zero);
                    return;
                }
                Internal_SetActiveScene(value.NativePointer);
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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_GetCamera", CallingConvention = CallingConvention.StdCall)]
        private static extern Camera Internal_GetCamera();

        public static Camera Camera
        {
            get
            {
                return Internal_GetCamera();
            }
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_Update", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Update();

        public void Update()
        {
            Internal_Update();
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_UpdateCamera", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Update(Camera camera);


        public static void UpdateCamera(Camera camera)
        {
            Internal_Update(camera);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_LoadFromModel", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_LoadFromModel(IntPtr scene, IntPtr model);


        public void LoadFromModel(Model model)
        {
            Internal_LoadFromModel(this.NativePointer, model.NativePointer);
        }


        
    }
}
