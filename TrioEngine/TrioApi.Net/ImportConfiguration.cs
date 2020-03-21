using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Net
{
    internal static class ImportConfiguration
    {
        private const string baseDirectory = @"";

#if TRIO_DIRECTX
        public const string DllImportFilename = baseDirectory + "TrioApi.dll";
#else
        public const string DllImportFilename = baseDirectory + "TrioApiGL.dll";
#endif
    }
}
