using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioApi.Net
{
    internal static class ImportConfiguration
    {
#if DEBUG
        private const string baseDirectory = @"";
#else
        private const string baseDirectory = @"";
        //private const string baseDirectory = @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\TrioEngine\TrioWpfEditor\bin\x64\Release\";
#endif

#if TRIO_DIRECTX
        public const string DllImportFilename = baseDirectory + "TrioApi.dll";
#else
        public const string DllImportFilename = baseDirectory + "TrioApiGL.dll";
#endif
    }
}
