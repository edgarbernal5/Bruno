using System;
using TrioApi.Net.Graphics.Core;

namespace EsteroFramework.Graphics.Editor
{
    public class EditorPrimitivesService : IEditorPrimitivesService
    {
        private readonly IGraphicsService m_graphicsService;

        public GridMesh GridMesh { get; set; }

        public EditorPrimitivesService(IGraphicsService graphicsService)
        {
            m_graphicsService = graphicsService;
        }

        public void InitializeMeshes()
        {
            var device = m_graphicsService.GraphicsDevice;
            GridMesh = new GridMesh(device, 64);
        }
    }
}
