using EsteroFramework.Graphics.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
