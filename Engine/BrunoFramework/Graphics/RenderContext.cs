
using BrunoFramework.Editor.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Interop;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;

namespace BrunoFramework.Graphics
{
    public class RenderContext
    {
        public IGameSurfaceTarget GameSurfaceTarget { get; set; }

        public GraphicsDevice GraphicsDevice { get; set; }

        public Viewport Viewport { get; set; }

        public RenderTarget2D RenderTarget { get; set; }

        public GameGraphicsScreen Screen { get; set; }

        public Camera Camera { get; set; }
    }
}
