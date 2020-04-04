using Estero.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class EffectTechnique : INamedItem
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectTechnique_GetPasses", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_GetPasses(IntPtr technique, ref IntPtr passes, ref int size);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectTechnique_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr technique);

        public string Name { get; private set; }

        internal IntPtr m_nativePtr;
        private EffectPassCollection m_passes;

        internal EffectTechnique(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
            LoadData();
        }

        private void LoadData()
        {
            LoadPasses();
            Name = Internal_GetName(m_nativePtr);
        }

        private void LoadPasses()
        {
            int total = 0;
            IntPtr array = IntPtr.Zero;
            Internal_GetPasses(m_nativePtr, ref array, ref total);

            if (total > 0)
            {
                IntPtr[] passPtrs = new IntPtr[total];
                Marshal.Copy(array, passPtrs, 0, total);

                var passes = new EffectPass[total];
                for (int i = 0; i < total; i++)
                {
                    passes[i] = new EffectPass(passPtrs[i]);
                }

                Marshal.FreeCoTaskMem(array);
                m_passes = new EffectPassCollection(passes);
            }
        }
    }
}
