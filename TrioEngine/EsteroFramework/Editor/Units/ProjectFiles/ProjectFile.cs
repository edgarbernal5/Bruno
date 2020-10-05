
using EsteroWindows;
using System;

namespace EsteroFramework.Editor.Units
{
    public abstract class ProjectFile : PropertyChangedBase, IDisposable
    {
        public bool IsModified { get; private set; }

        public bool IsDisposed { get; private set; }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

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

        public void New()
        {
            IsModified = false;
            OnNew();
        }

        protected abstract void OnNew();

        public void Load(string filename)
        {
            OnLoad(filename);
        }

        protected abstract void OnLoad(string filename);

        public void Save()
        {
            OnSave();
        }

        protected abstract void OnSave();
    }
}
