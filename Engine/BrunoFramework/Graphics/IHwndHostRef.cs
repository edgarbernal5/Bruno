
using System;

namespace BrunoFramework.Graphics
{
    public interface IHwndHostRef
    {
        void AddRef(IntPtr windowHandle, int width, int height);
        void RemoveRef(IntPtr windowHandle);
        void Reset(IntPtr windowHandle, int width, int height);
    }
}
