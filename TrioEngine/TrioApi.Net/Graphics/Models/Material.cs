using Estero.Interop;
using System;

namespace TrioApi.Net.Graphics
{
    public class Material : CppObject
    {
        internal Material(IntPtr nativePtr)
        {
            m_nativePointer = nativePtr;
        }

        protected override void OnDisposing(bool disposing)
        {

        }
    }
}
