
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Graphics.Interop;
using System.Collections.Generic;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Graphics
{
    public class GraphicsService : IGraphicsService
    {
        public GraphicsDevice GraphicsDevice => m_graphicsDevice;
        private GraphicsDevice m_graphicsDevice;

        public List<IGameSurfaceTarget> GameSurfaceTargets => m_gameSurfaceTargets;
        private List<IGameSurfaceTarget> m_gameSurfaceTargets;

        private List<GameGraphicsScreen> m_cachedScreens;

        private RenderContext m_renderContext;

        public GraphicsService(GraphicsDevice graphicsDevice)
        {
            m_graphicsDevice = graphicsDevice;
            m_gameSurfaceTargets = new List<IGameSurfaceTarget>();

            m_cachedScreens = new List<GameGraphicsScreen>();
            m_renderContext = new RenderContext();
        }

        public void Render(IGameSurfaceTarget surfaceTarget, IList<GameGraphicsScreen> graphicsScreens)
        {
            m_renderContext.GraphicsDevice = m_graphicsDevice;

            m_renderContext.GameSurfaceTarget = surfaceTarget;
            if (surfaceTarget.BeginRender(m_renderContext))
            {
                CacheScreens(graphicsScreens);

                var finalRenderTarget = m_renderContext.RenderTarget;
                var finalViewport = m_renderContext.Viewport;

                GraphicsDevice.Viewport = finalViewport;
                for (int i = 0; i < m_cachedScreens.Count; i++)
                {
                    var screen = m_cachedScreens[i];

                    screen.Render(m_renderContext);
                }
            }

            surfaceTarget.EndRender();
        }

        private void CacheScreens(IList<GameGraphicsScreen> graphicsScreens)
        {
            m_cachedScreens.Clear();
            for (int i = 0; i < graphicsScreens.Count; i++)
            {
                m_cachedScreens.Add(graphicsScreens[i]);
            }
        }
    }
}
