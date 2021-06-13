
using EsteroFramework.Editor.Game.Gizmos;
using System;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class AxisGizmoScaleRenderer : IAxisGizmoRenderer
    {
        private AxisGizmoScale[] m_gizmos;
        private ColorRGBA8[] m_colors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };
        private Matrix[] m_gizmoLocalWorlds;

        private Effect m_effect;
        private Matrix m_gizmoWorld;

        public AxisGizmoScaleRenderer(GraphicsDevice device)
        {
            m_gizmoLocalWorlds = new Matrix[3];
            m_gizmoLocalWorlds[0] = Matrix.CreateRotationZ(MathHelper.ToRadians(-90.0f)) * Matrix.CreateTranslation(Vector3.Right * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH) * 0.5f);
            m_gizmoLocalWorlds[1] = Matrix.CreateTranslation(Vector3.Up * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH) * 0.5f);
            m_gizmoLocalWorlds[2] = Matrix.CreateRotationX(MathHelper.ToRadians(90.0f)) * Matrix.CreateTranslation(Vector3.Backward * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH) * 0.5f);

            m_gizmos = new AxisGizmoScale[3];
            for (int i = 0; i < 3; i++)
            {
                m_gizmos[i] = new AxisGizmoScale(device, m_colors[i], GizmoService.CONE_HEIGHT, GizmoService.LINE_LENGTH);
            }

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
            m_gizmoWorld = Matrix.Identity;
        }

        public void Render(RenderContext renderContext)
        {
            var device = renderContext.GraphicsDevice;

            device.DepthStencilState = DepthStencilState.None;
            ////device->SetBlendState(BlendState::Opaque);
            device.RasterizerState = RasterizerState.CullCounterClockwise;

            for (int i = 0; i < 3; i++)
            {
                device.SetIndexBuffer(m_gizmos[i].IndexBuffer);
                device.SetVertexBuffer(m_gizmos[i].VertexBuffer);

                Matrix viewProjection = m_gizmoLocalWorlds[i] * m_gizmoWorld * renderContext.Camera.ViewProjection;

                m_effect.Parameters["gWorldViewProj"].SetValue(viewProjection);
                m_effect.Techniques[0].Passes[0].Apply();

                m_gizmos[i].Draw(device);
            }
        }

        public void SetWorld(Matrix gizmoWorld)
        {
            m_gizmoWorld = gizmoWorld;
        }
    }
}
