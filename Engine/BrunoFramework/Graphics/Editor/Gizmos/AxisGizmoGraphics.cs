
using BrunoFramework.Editor.Game.Gizmos;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoGraphics
    {
        private readonly float m_lineOffset;
        public VertexPositionColor[] TranslationLines
        {
            get => m_translationLinesVtx;
        }
        private VertexPositionColor[] m_translationLinesVtx = new VertexPositionColor[18];

        private ColorRGBA8[] m_color = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };

        public AxisGizmoGraphics(float lineOffset)
        {
            m_lineOffset = lineOffset;
        }

        public void ChangeColors(ColorRGBA8[] axisColors)
        {
            for (int i = 0; i < 6; i++)
            {
                m_translationLinesVtx[i].Color = axisColors[0];
            }
            for (int i = 6; i < 12; i++)
            {
                m_translationLinesVtx[i].Color = axisColors[1];
            }
            for (int i = 12; i < 18; i++)
            {
                m_translationLinesVtx[i].Color = axisColors[2];
            }
        }

        public void UpdateLinesFor(GizmoAxis gizmoAxis, float lineLength)
        {
            switch (gizmoAxis)
            {
                case GizmoAxis.X:
                    m_translationLinesVtx[0].Position = Vector3.Zero;
                    m_translationLinesVtx[1].Position = new Vector3(lineLength, 0, 0);

                    m_translationLinesVtx[2].Position = new Vector3(m_lineOffset, 0.0f, 0.0f);
                    m_translationLinesVtx[3].Position = new Vector3(m_lineOffset, 0.0f, m_lineOffset);

                    m_translationLinesVtx[4].Position = new Vector3(m_lineOffset, 0.0f, 0.0f);
                    m_translationLinesVtx[5].Position = new Vector3(m_lineOffset, m_lineOffset, 0.0f);

                    break;
                case GizmoAxis.Y:
                    m_translationLinesVtx[6].Position = Vector3.Zero;
                    m_translationLinesVtx[7].Position = new Vector3(0.0f, lineLength, 0.0f);

                    m_translationLinesVtx[8].Position = new Vector3(0.0f, m_lineOffset, 0.0f);
                    m_translationLinesVtx[9].Position = new Vector3(m_lineOffset, m_lineOffset, 0.0f);

                    m_translationLinesVtx[10].Position = new Vector3(0.0f, m_lineOffset, 0.0f);
                    m_translationLinesVtx[11].Position = new Vector3(0.0f, m_lineOffset, m_lineOffset);

                    break;
                case GizmoAxis.Z:
                    m_translationLinesVtx[12].Position = Vector3.Zero;
                    m_translationLinesVtx[13].Position = new Vector3(0.0f, 0.0f, lineLength);

                    m_translationLinesVtx[14].Position=new Vector3(0.0f, 0.0f, m_lineOffset);
                    m_translationLinesVtx[15].Position = new Vector3(0.0f, m_lineOffset, m_lineOffset);

                    m_translationLinesVtx[16].Position = new Vector3(0.0f, 0.0f, m_lineOffset);
                    m_translationLinesVtx[17].Position = new Vector3(m_lineOffset, 0.0f, m_lineOffset);

                    break;
                
            }
        }

        public void InitializeTranslation(float lineLength)
        {
            //X
            m_translationLinesVtx[0] = new VertexPositionColor(Vector3.Zero, m_color[0]);
            m_translationLinesVtx[1] = new VertexPositionColor(new Vector3(lineLength, 0.0f, 0.0f), m_color[0]);

            m_translationLinesVtx[2] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
            m_translationLinesVtx[3] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[0]);

            m_translationLinesVtx[4] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, 0.0f), m_color[0]);
            m_translationLinesVtx[5] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[0]);

            //Y
            m_translationLinesVtx[6] = new VertexPositionColor(Vector3.Zero, m_color[1]);
            m_translationLinesVtx[7] = new VertexPositionColor(new Vector3(0.0f, lineLength, 0.0f), m_color[1]);

            m_translationLinesVtx[8] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
            m_translationLinesVtx[9] = new VertexPositionColor(new Vector3(m_lineOffset, m_lineOffset, 0.0f), m_color[1]);

            m_translationLinesVtx[10] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, 0.0f), m_color[1]);
            m_translationLinesVtx[11] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[1]);

            //Z
            m_translationLinesVtx[12] = new VertexPositionColor(Vector3.Zero, m_color[2]);
            m_translationLinesVtx[13] = new VertexPositionColor(new Vector3(0.0f, 0.0f, lineLength), m_color[2]);

            m_translationLinesVtx[14] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
            m_translationLinesVtx[15] = new VertexPositionColor(new Vector3(0.0f, m_lineOffset, m_lineOffset), m_color[2]);

            m_translationLinesVtx[16] = new VertexPositionColor(new Vector3(0.0f, 0.0f, m_lineOffset), m_color[2]);
            m_translationLinesVtx[17] = new VertexPositionColor(new Vector3(m_lineOffset, 0.0f, m_lineOffset), m_color[2]);
        }
    }
}
