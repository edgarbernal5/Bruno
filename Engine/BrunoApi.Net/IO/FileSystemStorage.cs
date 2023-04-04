using Bruno.Interop;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;

namespace BrunoApi.Net.IO
{
    public class FileSystemStorage : CppObject, IStorage
    {
        private readonly string m_rootDirectory;

        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "FileSystemStorage_Ctor", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private static extern IntPtr Internal_ctor([MarshalAs(UnmanagedType.LPStr)] string rootDirectory);
        
        [DllImport(ImportConfiguration.DllImportFilename, EntryPoint = "FileSystemStorage_GetRootDirectory", CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string Internal_GetRootDirectory(IntPtr storage);

        public FileSystemStorage(string rootDirectory)
        {
            m_nativePointer = Internal_ctor(rootDirectory);

            m_rootDirectory = Internal_GetRootDirectory(m_nativePointer);
        }

        public virtual IEnumerable<string> GetDirectories(string path)
        {
            var normalizedPath = NormalizePath(Path.Combine(m_rootDirectory, path));

            return Directory.GetDirectories(normalizedPath)
                .Select(directory => GetRelativePath(NormalizePath(directory)));
        }

        public virtual IEnumerable<string> GetFiles(string path)
        {
            var normalizedPath = NormalizePath(Path.Combine(m_rootDirectory, path));

            return Directory.GetFiles(normalizedPath)
                .Select(directory => GetRelativePath(NormalizePath(directory)));
        }

        private string GetRelativePath(string absolutePath)
        {
            return absolutePath.Substring(m_rootDirectory.Length);
        }

        public virtual string GetRealPath(string path)
        {
            var realPath = Path.Combine(m_rootDirectory, path);

            return realPath;
        }

        private string NormalizePath(string path)
        {
            //const string message = "Invalid path.";

            //if (string.IsNullOrEmpty(path))
            //    throw new ArgumentException("Invalid path. Path must not be empty.", "path");

            //if (path.Contains(".."))
            //    throw new ArgumentException(message, "path");

            //path = path.Replace('\\', '/');

            //while (path.StartsWith("./", StringComparison.Ordinal))
            //    path = path.Substring(2);

            //path = path.Replace("/./", "/");

            //if (path.Length == 0)
            //    throw new ArgumentException(message, "path");

            //while (path.Length > 0 && path[path.Length - 1] == '/')
            //    path = path.Substring(0, path.Length - 1);

            return path;
        }

        public bool HasDirectories(string path)
        {
            var normalizedPath = NormalizePath(Path.Combine(m_rootDirectory, path));
            var directories = Directory.GetDirectories(normalizedPath);
            return directories != null && directories.Length > 0;
        }
    }
}
