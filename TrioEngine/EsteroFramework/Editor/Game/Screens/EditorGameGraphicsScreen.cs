
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Editor;
using System;
using TrioApi.Net.Graphics.Core;

namespace EsteroFramework.Editor.Graphics
{
    public class EditorGameGraphicsScreen : GameGraphicsScreen
    {
        public GridMesh GridMesh
        {
            get
            {
                return m_gridMesh;
            }
            set
            {
                m_gridMesh = value;
            }
        }
        private GridMesh m_gridMesh;

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
            renderContext.GraphicsDevice.Clear(ColorRGBA8.CornflowerBlue);

            //
            m_gridMesh.Render(renderContext);
        }
    }
}
