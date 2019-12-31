using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net.Content.Tasks
{
    public class BuildCoordinator : IDisposable
    {
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "BuildCoordinator_Ctor", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_ctor(
            [MarshalAs(UnmanagedType.LPStr)] string intermediateDirectory,
            [MarshalAs(UnmanagedType.LPStr)] string outputDirectory,
            [MarshalAs(UnmanagedType.LPStr)] string rootDirectory);

        bool disposed = false;

        internal IntPtr m_nativePtr;

        public BuildCoordinator(BuildCoordinatorSettings settings)
        {
            m_nativePtr = Internal_ctor(settings.IntermediateDirectory, settings.OutputDirectory, settings.RootDirectory);

        }
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (!disposed)
                {
                    disposed = true;
                    //Internal_dtor(m_nativePtr);
                }
            }
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }
    }
}
