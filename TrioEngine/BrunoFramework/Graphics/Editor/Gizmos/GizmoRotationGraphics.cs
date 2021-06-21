
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;
using System;
using System.Collections.Generic;

namespace BrunoFramework.Graphics.Editor.Gizmos
{
    public class GizmoRotationGraphics
    {
        private ColorRGBA8[] m_colors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };
        public const int RING_SEGMENTS = 32;

        public VertexPositionColor[] RedRingVertices
        {
            get => m_redRingVertices;
        }
        private VertexPositionColor[] m_redRingVertices;

        public VertexPositionColor[] GreenRingVertices
        {
            get => m_greenRingVertices;
        }
        private VertexPositionColor[] m_greenRingVertices;

        public VertexPositionColor[] BlueRingVertices
        {
            get => m_blueRingVertices;
        }
        private VertexPositionColor[] m_blueRingVertices;

        public GizmoRotationGraphics(float radius)
        {
            CreateMesh(radius);
        }

        private void CreateMesh(float radius)
        {
            CreateRingX(radius, m_colors[0]);
            CreateRingY(radius, m_colors[1]);
            CreateRingZ(radius, m_colors[2]);
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

            m_redRingVertices = vertices.ToArray();
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

            m_blueRingVertices = vertices.ToArray();
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

            m_greenRingVertices = vertices.ToArray();
        }
    }
}
