
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
        private static extern IntPtr Internal_Update(IntPtr scene);

        public void Update()
        {
            Internal_Update(m_nativePointer);
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
            Internal_LoadFromModel(NativePointer, model.NativePointer);
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct HierarchyComponentBridge
        {
            public long id;
            public long parentId;
            [MarshalAs(UnmanagedType.LPStr)]
            public string name;
        };

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Scene_GetHierarchies", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetHierarchies(IntPtr scene, ref int size, ref IntPtr collection);

        public HierarchyComponentBridge[] GetHierarchies()
        {
            int size = 0;
            IntPtr unmanagedArray = IntPtr.Zero;
            Internal_GetHierarchies(NativePointer, ref size, ref unmanagedArray);
            
            if (size > 0)
            {
                HierarchyComponentBridge[] outcome = new HierarchyComponentBridge[size];
                for (int i = 0; i < size; i++)
                {
                    IntPtr unmanagedItemPtr = unmanagedArray + Marshal.SizeOf<HierarchyComponentBridge>() * i;
                    outcome[i] = Marshal.PtrToStructure<HierarchyComponentBridge>(unmanagedItemPtr);

                    IntPtr unmanagedNameFieldPtr = unmanagedItemPtr + Marshal.SizeOf<long>() * 2;
                    IntPtr namePtr = Marshal.ReadIntPtr(unmanagedNameFieldPtr);
                    Marshal.FreeCoTaskMem(namePtr);
                }

                Marshal.FreeCoTaskMem(unmanagedArray);

                return outcome;
            }
            return new HierarchyComponentBridge[0];
        }
    }
}
