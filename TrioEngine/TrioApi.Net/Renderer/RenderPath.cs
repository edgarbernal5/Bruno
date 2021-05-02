using Estero.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Game;

namespace TrioApi.Net.Renderer
{
    public abstract class RenderPath : CppObject
    {
        private Scene m_scene;
        public Scene Scene { get { return m_scene; } }

        protected RenderPath(Scene scene)
        {
            m_scene = scene;
        }

        public abstract void Render();
    }
}
