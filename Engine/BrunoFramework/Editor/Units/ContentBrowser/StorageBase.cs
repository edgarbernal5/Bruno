
using Bruno.Interop;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public abstract class StorageBase : DisposableBase, IStorage
    {
        public abstract IEnumerable<string> GetDirectories(string path);
        public abstract IEnumerable<string> GetFiles(string path);
        public abstract string GetRealPath(string path);
    }
}
