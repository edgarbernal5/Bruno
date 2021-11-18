using System;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public abstract class Storage : IStorage, IDisposable
    {
        public bool IsDisposed { get; private set; }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public abstract IEnumerable<string> GetDirectories(string path);
        public abstract IEnumerable<string> GetFiles(string path);
        public abstract string GetRealPath(string path);

        protected virtual void Dispose(bool disposing)
        {
            if (!IsDisposed)
            {
                if (disposing)
                {

                }

                IsDisposed = true;
            }
        }
    }
}
