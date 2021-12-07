
namespace BrunoApi.Net
{
    internal static class ImportConfiguration
    {
#if DEBUG
        private const string baseDirectory = @"";
#else
        private const string baseDirectory = @"";
        //private const string baseDirectory = @"D:\Edgar\Documentos\Proyectos\CG\BrunoEngineGit\TrioEngine\BrunoWpfEditor\bin\x64\Release\";
#endif

#if BRUNO_DIRECTX
        public const string DllImportFilename = baseDirectory + "BrunoApi.dll";
#else
        public const string DllImportFilename = baseDirectory + "BrunoApiGL.dll";
#endif
    }
}
