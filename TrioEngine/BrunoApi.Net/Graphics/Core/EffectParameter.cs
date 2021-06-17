using Bruno.Collections;
using System;
using System.Runtime.InteropServices;
using BrunoApi.Net.Maths;

namespace BrunoApi.Net.Graphics.Core
{
    public class EffectParameter : INamedItem
    {
        public string Name { get; private set; }

        internal IntPtr m_nativePtr;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectParameter_GetName", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetName(IntPtr parameter);

        internal EffectParameter(IntPtr nativePtr)
        {
            m_nativePtr = nativePtr;
            Name = Internal_GetName(m_nativePtr);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectParameter_SetValueScalar", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetValueScalar(IntPtr parameter, float scalar);

        public void SetValue(float scalar)
        {
            Internal_SetValueScalar(m_nativePtr, scalar);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectParameter_SetValueMatrix", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetValueMatrix(IntPtr parameter, ref Matrix matrix);

        public void SetValue(Matrix matrix)
        {
            Internal_SetValueMatrix(m_nativePtr, ref matrix);
        }

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "EffectParameter_SetValueVector3", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_SetValueVector3(IntPtr parameter, ref Vector3 vector);

        public void SetValue(Vector3 vector)
        {
            Internal_SetValueVector3(m_nativePtr, ref vector);
        }
    }
}