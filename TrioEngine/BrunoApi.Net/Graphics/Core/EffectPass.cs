using Bruno.Collections;
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
{
    public class EffectPass : INamedItem
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectPass_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr pass);

        public string Name { get; private set; }

        internal IntPtr m_nativePtr;

        internal EffectPass(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
            Name = Internal_GetName(m_nativePtr);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectPass_Apply", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_Apply(IntPtr pass);

        public void Apply()
        {
            Internal_Apply(m_nativePtr);
        }
    }
}
