
using EsteroFramework.Editor.Game.Gizmos;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class AxisGizmoGraphics
    {
        private readonly float m_lineOffset;
        public VertexPositionColor[] translationLinesVtx = new VertexPositionColor[18];

        private ColorRGBA8[] m_color = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };

        public AxisGizmoGraphics(float lineOffset)
        {
            m_lineOffset = lineOffset;
        }

        public void ChangeColors(ColorRGBA8[] axisColors)
        {
            for (int i = 0; i < 6; i++)
            {
                translationLinesVtx[i].Color = axisColors[0];
            }
            for (int i = 6; i < 12; i++)
            {
                translationLinesVtx[i].Color = axisColors[1];
            }
            for (int i = 12; i < 18; i++)
            {
                translationLinesVtx[i].Color = axisColors[2];
            }
        }

        public void UpdateLinesFor(GizmoAxis gizmoAxis, float lineLength)
        {
            switch (gizmoAxis)
            {
                case GizmoAxis.X:
                    translationLinesVtx[0] = new VertexPositionColor(Vector3.Zero, m_color[0]);
                    translationLinesVtx[1] = new VertexPositionColor(new Vector3(lineLength, 0.0f, 0.0f), m_color[0]);

                    translationLinesVtx[2] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
                    translationLinesVtx[3] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[0]);

                    translationLinesVtx[4] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
                    translationLinesVtx[5] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[0]);

                    break;
                case GizmoAxis.Y:
                    translationLinesVtx[6] = new VertexPositionColor(Vector3.Zero, m_color[1]);
                    translationLinesVtx[7] = new VertexPositionColor(new Vector3(0.0f, lineLength, 0.0f), m_color[1]);

                    translationLinesVtx[8] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
                    translationLinesVtx[9] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[1]);

                    translationLinesVtx[10] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
                    translationLinesVtx[11] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[1]);

                    break;
                case GizmoAxis.Z:
                    translationLinesVtx[12] = new VertexPositionColor(Vector3.Zero, m_color[2]);
                    translationLinesVtx[13] = new VertexPositionColor(new Vector3(0.0f, 0.0f, lineLength), m_color[2]);

                    translationLinesVtx[14] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
                    translationLinesVtx[15] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[2]);

                    translationLinesVtx[16] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
                    translationLinesVtx[17] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[2]);

                    break;
                
            }
        }

        public void InitializeTranslation(float lineLength)
        {
            //X
            translationLinesVtx[0] = new VertexPositionColor(Vector3.Zero, m_color[0]);
            translationLinesVtx[1] = new VertexPositionColor(new Vector3(lineLength, 0.0f, 0.0f), m_color[0]);

            translationLinesVtx[2] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
            translationLinesVtx[3] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[0]);

            translationLinesVtx[4] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
            translationLinesVtx[5] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[0]);

            //Y
            translationLinesVtx[6] = new VertexPositionColor(Vector3.Zero, m_color[1]);
            translationLinesVtx[7] = new VertexPositionColor(new Vector3(0.0f, lineLength, 0.0f), m_color[1]);

            translationLinesVtx[8] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
            translationLinesVtx[9] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[1]);

            translationLinesVtx[10] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
            translationLinesVtx[11] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[1]);

            //Z
            translationLinesVtx[12] = new VertexPositionColor(Vector3.Zero, m_color[2]);
            translationLinesVtx[13] = new VertexPositionColor(new Vector3(0.0f, 0.0f, lineLength), m_color[2]);

            translationLinesVtx[14] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
            translationLinesVtx[15] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[2]);

            translationLinesVtx[16] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
            translationLinesVtx[17] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[2]);
        }
    }
}
