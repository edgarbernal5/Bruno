
using Bruno.Interop;
using System;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.Graphics.Core
{
    public class Effect : CppObject
    {
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

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_GetParameters", CallingConvention = CallingConvention.StdCall)]
        private static extern void Internal_GetParameters(IntPtr effect, ref IntPtr parameters, ref int size);

        public EffectParameterCollection Parameters
        {
            get
            {
                return m_parameters;
            }
        }
        private EffectParameterCollection m_parameters;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_Ctor", CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr Internal_Ctor(IntPtr device);

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "Effect_CompileEffectFromFile", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern void Internal_CompileEffectFromFile(IntPtr effect, string filename);

        public Effect(GraphicsDevice device, string filename)
        {
            m_nativePointer = Internal_Ctor(device.NativePointer);
            Internal_CompileEffectFromFile(m_nativePointer, filename);
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
            IntPtr unmanagedArray = IntPtr.Zero;
            Internal_GetParameters(m_nativePointer, ref unmanagedArray, ref total);

            if (total > 0)
            {
                IntPtr[] parametersPtrs = new IntPtr[total];
                Marshal.Copy(unmanagedArray, parametersPtrs, 0, total);

                var parameters = new EffectParameter[total];
                for (int i = 0; i < total; i++)
                {
                    parameters[i] = new EffectParameter(parametersPtrs[i]);
                }

                Marshal.FreeCoTaskMem(unmanagedArray);
                m_parameters = new EffectParameterCollection(parameters);
            }
        }

        private void LoadTechniques()
        {
            int total = 0;
            IntPtr unmanagedArray = IntPtr.Zero;
            Internal_GetTechniques(m_nativePointer, ref unmanagedArray, ref total);

            if (total > 0)
            {
                IntPtr[] techniquePtrs = new IntPtr[total];
                Marshal.Copy(unmanagedArray, techniquePtrs, 0, total);

                var techniques = new EffectTechnique[total];
                for (int i = 0; i < total; i++)
                {
                    techniques[i] = new EffectTechnique(techniquePtrs[i]);
                }

                Marshal.FreeCoTaskMem(unmanagedArray);
                m_technniques = new EffectTechniqueCollection(techniques);
            }
        }

        protected override void OnDisposing(bool disposing)
        {
        }
    }
}
