using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics
{
    public class GraphicsAdapter
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "GraphicsAdapter_GetAdapters", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetAdapters(ref IntPtr adapters, ref int size);

        private static GraphicsAdapter[] g_adapters;

        internal IntPtr m_nativePtr;

        static GraphicsAdapter()
        {
            int total = 0;
            IntPtr array = IntPtr.Zero;
            Internal_GetAdapters(ref array, ref total);

            if (total > 0)
            {
                IntPtr[] adapters = new IntPtr[total];
                Marshal.Copy(array, adapters, 0, total);

                g_adapters = new GraphicsAdapter[total];
                for (int i = 0; i < total; i++)
                {
                    g_adapters[i] = new GraphicsAdapter(adapters[i]);
                }

                Marshal.FreeCoTaskMem(array);
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
            m_nativePtr = nativePtr;
        }
    }
}
