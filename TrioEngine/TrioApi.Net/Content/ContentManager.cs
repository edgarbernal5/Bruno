using Estero.Interop;
using System;
using System.Reflection;
using System.Runtime.InteropServices;
using TrioApi.Net.Graphics;

namespace TrioApi.Net.Content
{
    public class ContentManager : CppObject
    {

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ContentManager_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device);


        public ContentManager(GraphicsDevice device)
            : base()
        {
            m_nativePointer = Internal_Ctor(device.NativePointer);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ContentManager_Ctor2", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_Ctor2(IntPtr device, [MarshalAs(UnmanagedType.LPStr)] string rootDirectory);

        public ContentManager(GraphicsDevice device, string rootDirectory)
            : base()
        {
            m_nativePointer = Internal_Ctor2(device.NativePointer, rootDirectory);
        }


        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "ContentManager_Load", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_Load(IntPtr contentManager, [MarshalAs(UnmanagedType.LPStr)] string assetName);

        public T Load<T>(string assetName) where T : class, new()
        {
            IntPtr nativeCall = Internal_Load(m_nativePointer, assetName);

            if (nativeCall == IntPtr.Zero)
                return default(T);

            Type type = typeof(T);

            BindingFlags flags = BindingFlags.NonPublic | BindingFlags.Instance;
            T instance = Activator.CreateInstance(type, flags, null, new object[] { nativeCall }, null) as T;

            return instance;
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
        }
    }
}
