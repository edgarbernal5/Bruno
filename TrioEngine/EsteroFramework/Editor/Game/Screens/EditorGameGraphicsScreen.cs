
using EsteroFramework.Graphics;
using System;
using TrioWpfFramework.Net.Graphics.Core;

namespace EsteroFramework.Editor.Graphics
{
    public class EditorGameGraphicsScreen : GameGraphicsScreen
    {
        public EditorGameGraphicsScreen() 
            : base()
        {

        }

        public override void Update(TimeSpan deltaTime)
        {

        }

        public override void Render(RenderContext renderContext)
        {
            renderContext.Screen = this;
            DoOnRender(renderContext);
            renderContext.Screen = null;
        }

        private void DoOnRender(RenderContext renderContext)
        {
            renderContext.GraphicsDevice.Clear(Color.Red);

            //
        }
    }
}
