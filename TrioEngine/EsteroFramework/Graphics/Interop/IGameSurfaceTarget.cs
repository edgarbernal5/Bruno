
using EsteroFramework.Editor.Graphics;
using System;
using System.Collections.Generic;

namespace EsteroFramework.Graphics.Interop
{
    public interface IGameSurfaceTarget
    {
        IntPtr Handle { get; }

        int Width { get; }

        int Height { get; }

        int LastWidth { get; set; }

        int LastHeight { get; set; }

        IList<GameGraphicsScreen> GameGraphicsScreens { get; }

        GameImageSource GameImageSource { get; }

        bool BeginRender(RenderContext context);
        void EndRender();
    }
}
