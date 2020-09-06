using System;
using System.Collections.Generic;

namespace EsteroFramework.Graphics.Interop
{
    public interface IGameSurfaceTarget
    {
        IntPtr Handle { get; }

        int Width { get; }

        int Height { get; }

        List<GameGraphicsScreen> GameGraphicsScreens { get; }

        GameImageSource GameImageSource { get; }

        bool BeginRender(RenderContext context);
        void EndRender();
    }
}
