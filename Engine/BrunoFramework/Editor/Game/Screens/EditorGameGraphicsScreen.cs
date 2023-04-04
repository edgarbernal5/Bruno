
using BrunoFramework.Editor.Game.Gizmos;
using BrunoFramework.Editor.Units;
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;
using BrunoApi.Net.Renderer;

namespace BrunoFramework.Editor.Graphics
{
    public class EditorGameGraphicsScreen : GameGraphicsScreen
    {
        public GridMesh GridMesh
        {
            get => m_gridMesh;
            set => m_gridMesh = value;
        }
        private GridMesh m_gridMesh;

        public bool ShowGrid
        {
            get => m_showGrid;
            set => m_showGrid = value;
        }
        private bool m_showGrid;

        public IGizmoService GizmoService
        {
            get => m_gizmoService;
            set => m_gizmoService = value;
        }
        private IGizmoService m_gizmoService;

        public RenderPath RenderPath
        {
            get => m_renderPath;
            set => m_renderPath = value;
        }
        private RenderPath m_renderPath;

        public DebugRenderer DebugRenderer
        {
            get => m_debugRenderer;
            set => m_debugRenderer = value;
        }
        private DebugRenderer m_debugRenderer;

        public EditorGameGraphicsScreen()
            : base()
        {
        }

        public override void Update(TimeSpan deltaTime)
        {
            //Scene.Update();
        }

        public override void Render(RenderContext renderContext)
        {
            if (m_renderPath == null) return;

            renderContext.Screen = this;
            DoOnRender(renderContext);
            renderContext.Screen = null;
        }

        private void DoOnRender(RenderContext renderContext)
        {
            BrunoApi.Net.Game.Scene.ActiveScene = Scene;
            Scene.Update();

            renderContext.GraphicsDevice.Clear(ColorRGBA8.CornflowerBlue);

            //
            Camera.Viewport = renderContext.Viewport;
            renderContext.Camera = Camera;

            BrunoApi.Net.Game.Scene.UpdateCamera(ConvertToCamera(Camera));
            m_renderPath.Render();

            if (m_showGrid)
            {
                m_gridMesh.Render(renderContext);
            }

            //draw bounding boxes
            m_debugRenderer.Render(renderContext);

            m_gizmoService.Render(renderContext);
        }

        private Renderer.Camera ConvertToCamera(Camera camera)
        {
            return new Renderer.Camera()
            {
                m_position = camera.Position,
                m_target = camera.Target,
                m_up = camera.Up,

                m_nearPlane = camera.NearPlane,
                m_farPlane = camera.FarPlane,
                m_fieldOfView = camera.FieldOfView,
                m_aspectRatio = camera.AspectRatio
            };
        }
    }
}
