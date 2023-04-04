
using System;

namespace Bruno.Interop
{
    public abstract class CppObject : DisposableBase, ICppObject
    {
        public IntPtr NativePointer => m_nativePointer;
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

    public interface ICppObject
    {
        IntPtr NativePointer { get; }
    }
}
