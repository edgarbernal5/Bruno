
using BrunoFramework.Editor.Game.Gizmos;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoScaleRenderer : IAxisGizmoRenderer
    {
        public AxisGizmoScale AxisGizmoScale
        {
            get => m_gizmoScale;
        }
        private AxisGizmoScale m_gizmoScale;
        private AxisGizmoGraphics m_axisGizmoGraphics;
        private ColorRGBA8[] m_axisColors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };

        private Effect m_effect;
        private Matrix m_gizmoWorld;

        private float m_currentLineLength;

        public AxisGizmoScaleRenderer(GraphicsDevice device, ColorRGBA8[] axisColors, GizmoConfig gizmoConfig)
        {
            m_axisGizmoGraphics = new AxisGizmoGraphics(gizmoConfig.LINE_OFFSET);
            m_axisGizmoGraphics.InitializeTranslation(gizmoConfig.LINE_LENGTH);
            m_axisColors = axisColors;

            m_gizmoScale = new AxisGizmoScale(device, m_axisColors, gizmoConfig.CONE_HEIGHT, gizmoConfig.LINE_LENGTH);

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
            m_gizmoWorld = Matrix.Identity;
            m_currentLineLength = gizmoConfig.LINE_LENGTH;
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

            device.DrawUserPrimitives(PrimitiveType.LineList, m_axisGizmoGraphics.TranslationLines, 0,
                m_axisGizmoGraphics.TranslationLines.Length / 2);

            m_gizmoScale.Draw(device);
        }

        public void SetWorld(Matrix gizmoWorld)
        {
            m_gizmoWorld = gizmoWorld;
        }

        public void RestoreGizmo(GizmoAxis gizmoAxis, float length)
        {
            m_currentLineLength = length;
            m_axisGizmoGraphics.UpdateLinesFor(gizmoAxis, m_currentLineLength);

            m_gizmoScale.PutBackBox(gizmoAxis);
        }

        public void UpdateGizmoLength(GizmoAxis gizmoAxis, Vector3 delta)
        {
            var scalar = gizmoAxis == GizmoAxis.X ? delta.X : (gizmoAxis == GizmoAxis.Y ? delta.Y : delta.Z);
            m_currentLineLength += scalar;
            m_axisGizmoGraphics.UpdateLinesFor(gizmoAxis, m_currentLineLength);

            m_gizmoScale.PutOffsetInVertices(delta, gizmoAxis);
        }

        public void SetColors(ColorRGBA8[] axisColors)
        {
            m_axisGizmoGraphics.ChangeColors(axisColors);
        }
    }
}
