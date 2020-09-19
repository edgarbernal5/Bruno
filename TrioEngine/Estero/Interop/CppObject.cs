
using System;

namespace Estero.Interop
{
    public abstract class CppObject : DisposableBase
    {
        public IntPtr NativePointer
        {
            get
            {
                return m_nativePtr;
            }
        }
        protected IntPtr m_nativePtr;

        public CppObject(IntPtr nativePointer)
        {
            m_nativePtr = nativePointer;
        }
        protected CppObject()
        {
            m_nativePtr = IntPtr.Zero;
        }

        protected override void OnDisposed()
        {
            m_nativePtr = IntPtr.Zero;
        }
    }
}
