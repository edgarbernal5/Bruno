using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoFramework.Editor.Units
{
    public interface IStorage
    {
        string GetRealPath(string path);
        IEnumerable<string> GetFiles(string path);
        IEnumerable<string> GetDirectories(string path);
    }
}
