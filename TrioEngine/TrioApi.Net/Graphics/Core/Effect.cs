
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Graphics.Core
{
    public class Effect : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_GetParameters", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetParameters(IntPtr effect, ref IntPtr parameters, ref int size);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_GetTechniques", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetTechniques(IntPtr effect, ref IntPtr techniques, ref int size);

        public EffectTechniqueCollection Techniques
        {
            get
            {
                return m_technniques;
            }
        }
        private EffectTechniqueCollection m_technniques;

        public EffectParameterCollection Parameters
        {
            get
            {
                return m_parameters;
            }
        }
        private EffectParameterCollection m_parameters;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_CompileEffectFromFile", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void Internal_CompileEffectFromFile(IntPtr effect, string filename);

        public Effect(GraphicsDevice device, string filename)
        {
            m_nativePtr = Internal_Ctor(device.NativePointer);
            Internal_CompileEffectFromFile(m_nativePtr, filename);
            LoadData();
        }

        private void LoadData()
        {
            LoadTechniques();
            LoadParameters();
        }

        private void LoadParameters()
        {
            int total = 0;
            IntPtr array = IntPtr.Zero;
            Internal_GetParameters(m_nativePtr, ref array, ref total);

            if (total > 0)
            {
                IntPtr[] parametersPtrs = new IntPtr[total];
                Marshal.Copy(array, parametersPtrs, 0, total);

                var parameters = new EffectParameter[total];
                for (int i = 0; i < total; i++)
                {
                    parameters[i] = new EffectParameter(parametersPtrs[i]);
                }

                Marshal.FreeCoTaskMem(array);
                m_parameters = new EffectParameterCollection(parameters);
            }
        }

        private void LoadTechniques()
        {
            int total = 0;
            IntPtr array = IntPtr.Zero;
            Internal_GetTechniques(m_nativePtr, ref array, ref total);

            if (total > 0)
            {
                IntPtr[] techniquePtrs = new IntPtr[total];
                Marshal.Copy(array, techniquePtrs, 0, total);

                var techniques = new EffectTechnique[total];
                for (int i = 0; i < total; i++)
                {
                    techniques[i] = new EffectTechnique(techniquePtrs[i]);
                }

                Marshal.FreeCoTaskMem(array);
                m_technniques = new EffectTechniqueCollection(techniques);
            }
        }

        protected override void OnDisposing(bool disposing)
        {
        }
    }
}
