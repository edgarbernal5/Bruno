using EsteroFramework.Graphics;
using System;
using TrioApi.Net.Graphics.Core;

namespace EsteroFramework.Editor.Graphics
{
    public class EmptyGameGraphicsScreen : GameGraphicsScreen
    {
        public override void Render(RenderContext renderContext)
        {
            renderContext.GraphicsDevice.Clear(Color.Red);
        }

        public override void Update(TimeSpan deltaTime)
        {

        }
    }
}
