
using Bruno.Interop;
using System.Collections.Generic;

namespace BrunoApi.Net.IO
{
    public interface IStorage : ICppObject
    {
        string GetRealPath(string path);
        IEnumerable<string> GetFiles(string path);
        IEnumerable<string> GetDirectories(string path);
        bool HasDirectories(string path);
    }
}
