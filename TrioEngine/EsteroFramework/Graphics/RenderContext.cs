
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Graphics.Interop;
using TrioApi.Net.Graphics.Core;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Graphics
{
    public class RenderContext
    {
        public IGameSurfaceTarget GameSurfaceTarget { get; set; }

        public GraphicsDevice GraphicsDevice { get; set; }

        public Viewport Viewport { get; set; }
        public RenderTarget2D RenderTarget { get; set; }
        public GameGraphicsScreen Screen { get; set; }
    }
}
