
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Editor;

namespace EsteroFramework.Editor.Game.Gizmos
{
    public class GizmoService : IGizmoService
    {
        public IAxisGizmoRenderer AxisGizmoRenderer
        {
            get
            {
                return m_axisGizmoRenderer;
            }
            set
            {
                m_axisGizmoRenderer = value;
            }
        }
        private IAxisGizmoRenderer m_axisGizmoRenderer;

        private IGraphicsService m_graphicsService;

        public GizmoService(IGraphicsService graphicsService)
        {
            m_graphicsService = graphicsService;
            InitializeGizmos();
        }

        private void InitializeGizmos()
        {
            m_axisGizmoRenderer = new AxisGizmoRenderer(m_graphicsService.GraphicsDevice);
        }
    }

    public interface IGizmoService
    {
        IAxisGizmoRenderer AxisGizmoRenderer { get; }
    }
}
