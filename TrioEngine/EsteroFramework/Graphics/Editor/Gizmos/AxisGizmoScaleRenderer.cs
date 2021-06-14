
using EsteroFramework.Editor.Game.Gizmos;
using System;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class AxisGizmoScaleRenderer : IAxisGizmoRenderer
    {
        public AxisGizmoScale AxisGizmoScale
        {
            get => m_gizmoScale;
        }
        private AxisGizmoScale m_gizmoScale;
        private AxisGizmoGraphics m_axisGizmoGraphics;
        private ColorRGBA8[] m_colors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };

        private Effect m_effect;
        private Matrix m_gizmoWorld;

        public AxisGizmoScaleRenderer(GraphicsDevice device)
        {
            m_axisGizmoGraphics = new AxisGizmoGraphics(GizmoService.LINE_OFFSET);
            m_axisGizmoGraphics.InitializeTranslation(GizmoService.LINE_LENGTH);

            m_gizmoScale = new AxisGizmoScale(device, m_colors, GizmoService.CONE_HEIGHT, GizmoService.LINE_LENGTH);

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
            m_gizmoWorld = Matrix.Identity;
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

            device.DrawUserPrimitives(PrimitiveType.LineList, m_axisGizmoGraphics.translationLinesVtx, 0,
                m_axisGizmoGraphics.translationLinesVtx.Length / 2);

            m_gizmoScale.Draw(device);
        }

        public void SetWorld(Matrix gizmoWorld)
        {
            m_gizmoWorld = gizmoWorld;
        }

        public void ChangeLineLength(GizmoAxis gizmoAxis, float lineLength)
        {
            m_axisGizmoGraphics.UpdateLinesFor(gizmoAxis, lineLength);
        }
    }
}
