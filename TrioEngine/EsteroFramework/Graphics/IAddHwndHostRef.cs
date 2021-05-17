
using System;

namespace EsteroFramework.Graphics
{
    public interface IAddHwndHostRef
    {
        bool IsReady { get; }
        void AddHostRef(IntPtr windowHandle, int width, int height);
        void Reset(int width, int height);
    }
}
