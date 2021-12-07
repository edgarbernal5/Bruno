using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace BrunoFramework.Editor.Units
{
    public class FileSystemStorage : StorageBase
    {
        private readonly string m_rootDirectory;

        public FileSystemStorage(string rootDirectory)
        {
            if (string.IsNullOrEmpty(rootDirectory))
            {
                m_rootDirectory = Directory.GetCurrentDirectory();
            }
            else
            {
                m_rootDirectory = Path.GetFullPath(rootDirectory);
            }

            m_rootDirectory = NormalizePath(m_rootDirectory);
        }

        public override IEnumerable<string> GetDirectories(string path)
        {
            var normalizedPath = NormalizePath(Path.Combine(m_rootDirectory, path));

            return Directory.GetDirectories(normalizedPath)
                .Select(directory => NormalizePath(directory));
        }

        public override IEnumerable<string> GetFiles(string path)
        {
            var normalizedPath = NormalizePath(Path.Combine(m_rootDirectory, path));

            return Directory.GetFiles(normalizedPath)
                .Select(directory => NormalizePath(directory));
        }

        public override string GetRealPath(string path)
        {
            throw new NotImplementedException();
        }

        private string NormalizePath(string path)
        {
            const string message = "Invalid path.";

            if (string.IsNullOrEmpty(path))
                throw new ArgumentException("Invalid path. Path must not be empty.", "path");

            if (path.Contains(".."))
                throw new ArgumentException(message, "path");

            path = path.Replace('\\', '/');

            while (path.StartsWith("./", StringComparison.Ordinal))
                path = path.Substring(2);

            path = path.Replace("/./", "/");

            if (path.Length == 0)
                throw new ArgumentException(message, "path");

            while (path.Length > 0 && path[path.Length - 1] == '/')
                path = path.Substring(0, path.Length - 1);

            return path;
        }
    }
}
