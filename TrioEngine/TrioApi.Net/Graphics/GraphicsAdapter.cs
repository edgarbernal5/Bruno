
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics
{
    public class GraphicsAdapter : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsAdapter_GetAdapters", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetAdapters(ref IntPtr adapters, ref int size);

        private static GraphicsAdapter[] g_adapters;

        static GraphicsAdapter()
        {
            int total = 0;
            IntPtr unmanagedArray = IntPtr.Zero;
            Internal_GetAdapters(ref unmanagedArray, ref total);

            if (total > 0)
            {
                IntPtr[] unmanagedAdapters = new IntPtr[total];
                Marshal.Copy(unmanagedArray, unmanagedAdapters, 0, total);

                g_adapters = new GraphicsAdapter[total];
                for (int i = 0; i < total; i++)
                {
                    g_adapters[i] = new GraphicsAdapter(unmanagedAdapters[i]);
                }

                Marshal.FreeCoTaskMem(unmanagedArray);
            }
        }

        public static GraphicsAdapter DefaultAdapter
        {
            get
            {
                return g_adapters[0];
            }
        }

        internal GraphicsAdapter(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        protected override void OnDisposing(bool disposing)
        {
        }
    }
}
