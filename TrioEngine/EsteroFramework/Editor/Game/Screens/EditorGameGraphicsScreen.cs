
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;
using TrioApi.Net.Renderer;

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

        public RenderPath RenderPath
        {
            get
            {
                return m_renderPath;
            }
            set
            {
                m_renderPath = value;
            }
        }
        private RenderPath m_renderPath;

        public EditorGameGraphicsScreen() 
            : base()
        {
            
        }

        public override void Update(TimeSpan deltaTime)
        {
            //Scene.Update
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
            renderContext.Camera = Camera;

            //float time = (float)GameUnit.m_gameStepTimer.TotalTime.TotalSeconds;
            //Camera.Rotation = Quaternion.CreateFromYawPitchRoll(time * 0.1f, -0.3f, 0);

            //Camera.Recalculate();

            m_gridMesh.Render(renderContext);

            TrioApi.Net.Game.Scene.UpdateCamera(ConvertToCamera(Camera));
            m_renderPath.Render();
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
