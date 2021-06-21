
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;
using BrunoFramework.Graphics.Editor.Gizmos;
using System;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoRotationRenderer : IAxisGizmoRenderer
    {
        private GizmoRotationGraphics m_gizmoRotationGraphics;

        private Matrix m_gizmoWorld;
        private Effect m_effect;

        public AxisGizmoRotationRenderer(GraphicsDevice device, float radius)
        {
            m_gizmoRotationGraphics = new GizmoRotationGraphics(radius);
            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
        }

        public void Render(RenderContext renderContext)
        {
            var device = renderContext.GraphicsDevice;

            device.DepthStencilState = DepthStencilState.None;
            ////device->SetBlendState(BlendState::Opaque);
            device.RasterizerState = RasterizerState.CullNone;

            Matrix viewProjection = m_gizmoWorld * renderContext.Camera.ViewProjection;

            m_effect.Parameters["gWorldViewProj"].SetValue(viewProjection);
            m_effect.Techniques[0].Passes[0].Apply();

            device.DrawUserPrimitives(PrimitiveType.LineStrip, m_gizmoRotationGraphics.RedRingVertices, 0,
               m_gizmoRotationGraphics.RedRingVertices.Length - 1);

            device.DrawUserPrimitives(PrimitiveType.LineStrip, m_gizmoRotationGraphics.GreenRingVertices, 0,
                m_gizmoRotationGraphics.GreenRingVertices.Length - 1);

            device.DrawUserPrimitives(PrimitiveType.LineStrip, m_gizmoRotationGraphics.BlueRingVertices, 0,
               m_gizmoRotationGraphics.BlueRingVertices.Length - 1);            
        }

        public void SetWorld(Matrix gizmoWorld)
        {
            m_gizmoWorld = gizmoWorld;
        }
    }
}
