
using System;

namespace Estero.Interop
{
    public abstract class DisposableBase : IDisposable
    {
        public bool IsDisposed
        {
            get
            {
                return m_isDisposed;
            }
        }
        protected bool m_isDisposed;

        protected DisposableBase()
        {
            m_isDisposed = false;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (m_isDisposed)
                return;

            OnDisposing(disposing);
            OnDisposed();
            m_isDisposed = true;
        }

        protected abstract void OnDisposing(bool disposing);

        protected virtual void OnDisposed() { }
    }
}
