
using System;

namespace Bruno.Interop
{
    public abstract class CppObject : DisposableBase
    {
        public IntPtr NativePointer
        {
            get
            {
                return m_nativePointer;
            }
        }
        protected IntPtr m_nativePointer;

        public CppObject(IntPtr nativePointer)
        {
            m_nativePointer = nativePointer;
        }
        protected CppObject()
        {
            m_nativePointer = IntPtr.Zero;
        }

        protected override void OnDisposed()
        {
            m_nativePointer = IntPtr.Zero;
        }
    }
}
