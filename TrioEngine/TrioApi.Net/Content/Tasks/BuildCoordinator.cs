
using Estero.Interop;
using System;
using System.Runtime.InteropServices;

namespace TrioApi.Net.Content.Tasks
{
    public class BuildCoordinator : CppObject
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BuildCoordinator_Ctor", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_ctor(
            [MarshalAs(UnmanagedType.LPStr)] string intermediateDirectory,
            [MarshalAs(UnmanagedType.LPStr)] string outputDirectory,
            [MarshalAs(UnmanagedType.LPStr)] string rootDirectory);


        public BuildCoordinator(BuildCoordinatorSettings settings)
        {
            m_nativePtr = Internal_ctor(settings.IntermediateDirectory, settings.OutputDirectory, settings.RootDirectory);
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
            }
            //Internal_dtor(m_nativePtr);
        }
    }
}
