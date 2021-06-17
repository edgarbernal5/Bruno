using BrunoFramework.Graphics;
using System;
using BrunoApi.Net.Graphics.Core;

namespace BrunoFramework.Editor.Graphics
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
