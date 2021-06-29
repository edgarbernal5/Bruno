
using BrunoFramework.Editor.Game.Gizmos;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoTranslationRenderer : IAxisGizmoRenderer
    {
        private AxisGizmoTranslation[] m_gizmos;
        private ColorRGBA8[] m_axisColors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };
        private Matrix[] m_gizmoLocalWorlds;

        private Effect m_effect;
        private Matrix m_gizmoWorld;
        private AxisGizmoGraphics m_axisGizmoGraphics;

        public AxisGizmoTranslationRenderer(GraphicsDevice device, ColorRGBA8[] axisColors)
        {
            m_axisGizmoGraphics = new AxisGizmoGraphics(GizmoService.LINE_OFFSET);
            m_axisGizmoGraphics.InitializeTranslation(GizmoService.LINE_LENGTH);
            m_axisColors = axisColors;

            m_gizmoLocalWorlds = new Matrix[3];
            m_gizmoLocalWorlds[0] = Matrix.CreateRotationZ(MathHelper.ToRadians(-90.0f)) * Matrix.CreateTranslation(Vector3.Right * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH) * 0.5f);
            m_gizmoLocalWorlds[1] = Matrix.CreateTranslation(Vector3.Up * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH)* 0.5f);
            m_gizmoLocalWorlds[2] = Matrix.CreateRotationX(MathHelper.ToRadians(90.0f)) * Matrix.CreateTranslation(Vector3.Backward * (GizmoService.CONE_HEIGHT + GizmoService.LINE_LENGTH) * 0.5f);

            m_gizmos = new AxisGizmoTranslation[3];
            for (int i = 0; i < 3; i++)
            {
                m_gizmos[i] = new AxisGizmoTranslation(device, m_axisColors[i], GizmoService.CONE_HEIGHT, GizmoService.CONE_RADIUS, GizmoService.LINE_LENGTH);
            }

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
            m_gizmoWorld = Matrix.Identity;
        }

        public void SetWorld(Matrix gizmoWorld)
        {
            m_gizmoWorld = gizmoWorld;
        }



        public void Render(RenderContext renderContext)
        {
            var device = renderContext.GraphicsDevice;

            device.DepthStencilState = DepthStencilState.None;
            //device->SetBlendState(BlendState::Opaque);
            device.RasterizerState = RasterizerState.CullNone;

            Matrix worldViewProjection = m_gizmoWorld * renderContext.Camera.ViewProjection;
            for (int i = 0; i < 3; i++)
            {
                device.SetIndexBuffer(m_gizmos[i].IndexBuffer);
                device.SetVertexBuffer(m_gizmos[i].VertexBuffer);

                Matrix viewProjection = m_gizmoLocalWorlds[i] * worldViewProjection;

                m_effect.Parameters["gWorldViewProj"].SetValue(viewProjection);
                m_effect.Techniques[0].Passes[0].Apply();

                m_gizmos[i].Draw(device);
            }

            m_effect.Parameters["gWorldViewProj"].SetValue(worldViewProjection);
            m_effect.Techniques[0].Passes[0].Apply();

            device.DrawUserPrimitives(PrimitiveType.LineList, m_axisGizmoGraphics.TranslationLines, 0,
                m_axisGizmoGraphics.TranslationLines.Length / 2);
        }

        public void SetColor(ColorRGBA8[] axisColors)
        {
            m_axisGizmoGraphics.ChangeColors(axisColors);
        }
    }
}
