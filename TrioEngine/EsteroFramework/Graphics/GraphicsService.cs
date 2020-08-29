using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Graphics
{
    public class GraphicsService : IGraphicsService
    {
        private GraphicsDevice m_graphicsDevice;

        public GraphicsService(GraphicsDevice graphicsDevice)
        {
            m_graphicsDevice = graphicsDevice;
        }

        public GraphicsDevice GraphicsDevice => m_graphicsDevice;
    }
}
