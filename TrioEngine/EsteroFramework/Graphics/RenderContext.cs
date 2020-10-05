
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;

namespace EsteroFramework.Graphics
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
