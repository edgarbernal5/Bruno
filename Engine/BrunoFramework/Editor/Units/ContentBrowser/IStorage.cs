
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public interface IStorage
    {
        string GetRealPath(string path);
        IEnumerable<string> GetFiles(string path);
        IEnumerable<string> GetDirectories(string path);
    }
}
