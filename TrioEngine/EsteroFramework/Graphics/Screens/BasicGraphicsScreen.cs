using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net.Graphics.Core;

namespace EsteroFramework.Graphics
{
    public class BasicGraphicsScreen : GameGraphicsScreen
    {
        public BasicGraphicsScreen() : base()
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
        }
    }
}
