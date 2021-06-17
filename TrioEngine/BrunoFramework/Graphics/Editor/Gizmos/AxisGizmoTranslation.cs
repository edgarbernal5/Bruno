
using Bruno.Interop;
using System;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoTranslation : DisposableBase
    {
        public VertexBuffer VertexBuffer
        {
            get { return m_vertexBuffer; }
        }
        private VertexBuffer m_vertexBuffer;

        public IndexBuffer IndexBuffer
        {
            get { return m_indexBuffer; }
        }
        private IndexBuffer m_indexBuffer;

        private ColorRGBA8 m_color;

        public uint PrimitiveCount
        {
            get { return m_primitiveCount; }
        }
        private uint m_primitiveCount;
        private uint m_coneIndicesTotal;

        public AxisGizmoTranslation(GraphicsDevice device, ColorRGBA8 color, float coneHeight, float coneRadius, float lineLength)
        {
            m_color = color;

            CreateMeshes(device, coneHeight, coneRadius, lineLength);
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                m_indexBuffer.Dispose();
                m_vertexBuffer.Dispose();
            }
        }

        private void CreateMeshes(GraphicsDevice device, float coneHeight, float coneRadius, float lineLength)
        {
            const int subdivisions = 10;

            int coneVertices = subdivisions + 2;
            int totalVertices = coneVertices;

            int coneIndices = (subdivisions * 2) * 3;
            int totalIndices = coneIndices;
            m_coneIndicesTotal = (uint)coneIndices;

            VertexPositionColor[] vertices = new VertexPositionColor[totalVertices];
            int[] triangles = new int[totalIndices];

            m_vertexBuffer = new VertexBuffer(device, VertexPositionColor.VertexDeclaration, totalVertices);
            m_indexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, totalIndices);

            CreateCone(vertices, triangles, subdivisions, coneRadius, coneHeight, lineLength, 0, 0);

            m_primitiveCount = m_coneIndicesTotal / 3;

            m_vertexBuffer.SetData(vertices);
            m_indexBuffer.SetData(triangles);
        }

        private void CreateCone(VertexPositionColor[] vertices, int[] triangles, int subdivisions, float radius, float height, float lineLength, int verticesOffset, int indicesOffset)
        {
            float heightOffset = (lineLength - height) * 0.5f;
            vertices[verticesOffset] = new VertexPositionColor(new Vector3(0, heightOffset, 0), m_color);
            for (int i = 0, n = subdivisions - 1; i < subdivisions; i++)
            {
                float ratio = (float)i / n;
                float r = ratio * (MathHelper.PI * 2.0f);
                float x = (float)Math.Cos(r) * radius;
                float z = (float)Math.Sin(r) * radius;
                vertices[verticesOffset + i + 1] = new VertexPositionColor(new Vector3(x, heightOffset, z), m_color);
            }
            vertices[verticesOffset + subdivisions + 1] = new VertexPositionColor(new Vector3(0f, height + heightOffset, 0f), m_color);

            for (int i = 0, n = subdivisions - 1; i < n; i++)
            {
                int offset = i * 3;
                triangles[indicesOffset + offset] = 0;
                triangles[indicesOffset + offset + 1] = i + 1;
                triangles[indicesOffset + offset + 2] = i + 2;
            }

            int bottomOffset = subdivisions * 3;
            for (int i = 0, n = subdivisions - 1; i < n; i++)
            {
                int offset = i * 3 + bottomOffset;
                triangles[indicesOffset + offset] = i + 1;
                triangles[indicesOffset + offset + 1] = subdivisions + 1;
                triangles[indicesOffset + offset + 2] = i + 2;
            }
        }

        public void Draw(GraphicsDevice device)
        {
            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 0, 0, PrimitiveCount);
        }
    }
}
