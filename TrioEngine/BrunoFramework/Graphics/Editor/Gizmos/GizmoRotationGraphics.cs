
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;
using System;
using System.Collections.Generic;

namespace BrunoFramework.Graphics.Editor.Gizmos
{
    public class GizmoRotationGraphics
    {
        private ColorRGBA8[] m_axisColors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };
        public const int RING_SEGMENTS = 32;

        public VertexPositionColor[] RedRingVertices
        {
            get => m_yRingVertices;
        }
        private VertexPositionColor[] m_yRingVertices;

        public VertexPositionColor[] GreenRingVertices
        {
            get => m_zRingVertices;
        }
        private VertexPositionColor[] m_zRingVertices;

        public VertexPositionColor[] BlueRingVertices
        {
            get => m_xRingVertices;
        }
        private VertexPositionColor[] m_xRingVertices;

        public GizmoRotationGraphics(float radius, ColorRGBA8[] axisColors)
        {
            m_axisColors = axisColors;
            CreateMesh(radius);
        }

        public void SetColors(ColorRGBA8[] axisColors)
        {
            for (int i = 0; i < m_yRingVertices.Length; i++)
            {
                m_yRingVertices[i].Color = axisColors[0];
            }
            for (int i = 0; i < m_zRingVertices.Length; i++)
            {
                m_zRingVertices[i].Color = axisColors[2];
            }
            for (int i = 0; i < m_xRingVertices.Length; i++)
            {
                m_xRingVertices[i].Color = axisColors[1];
            }
        }

        private void CreateMesh(float radius)
        {
            CreateRingX(radius, m_axisColors[0]);
            CreateRingY(radius, m_axisColors[1]);
            CreateRingZ(radius, m_axisColors[2]);
        }

        private float GetCircleAngleFor(float segment)
        {
            return MathHelper.TwoPi * segment / RING_SEGMENTS;
        }
        /*
         x vertices.Add(new VertexPositionColor(new Vector3(sin * radius, 0, cos * radius), color));
        y vertices.Add(new VertexPositionColor(new Vector3(0, sin * radius, cos * radius), color));
        z  vertices.Add(new VertexPositionColor(new Vector3(sin * radius, cos * radius, 0), color));
         */
        private void CreateRingX(float radius, ColorRGBA8 color)
        {
            //y
            var vertices = new List<VertexPositionColor>();
            for (int i = 0; i < RING_SEGMENTS; i++)
            {
                float t = GetCircleAngleFor(i);

                float sin = (float)Math.Sin(t);
                float cos = (float)Math.Cos(t);
                vertices.Add(new VertexPositionColor(new Vector3(0, sin * radius, cos * radius), color));
            }
            vertices.Add(vertices[0]);

            m_yRingVertices = vertices.ToArray();
        }

        private void CreateRingY(float radius, ColorRGBA8 color)
        {
            //x
            var vertices = new List<VertexPositionColor>();

            for (int i = 0; i < RING_SEGMENTS; i++)
            {
                float t = GetCircleAngleFor(i);

                float sin = (float)Math.Sin(t);
                float cos = (float)Math.Cos(t);
                vertices.Add(new VertexPositionColor(new Vector3(sin * radius, 0, cos * radius), color));
            }
            vertices.Add(vertices[0]);

            m_xRingVertices = vertices.ToArray();
        }

        private void CreateRingZ(float radius, ColorRGBA8 color)
        {
            var vertices = new List<VertexPositionColor>();

            for (int i = 0; i < RING_SEGMENTS; i++)
            {
                float t = GetCircleAngleFor(i);

                float sin = (float)Math.Sin(t);
                float cos = (float)Math.Cos(t);
                vertices.Add(new VertexPositionColor(new Vector3(sin * radius, cos * radius, 0), color));
            }
            vertices.Add(vertices[0]);

            m_zRingVertices = vertices.ToArray();
        }
    }
}
