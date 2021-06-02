
using Estero.Interop;
using System;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class AxisGizmo : DisposableBase
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
        private uint m_coneVerticesTotal;
        private uint m_coneIndicesTotal;

        public AxisGizmo(GraphicsDevice device, ColorRGBA8 color, float coneHeight, float coneRadius, float lineLength)
        {
            m_color = color;

            CreateMeshes(device, coneHeight, coneRadius, lineLength);
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
        }

        private void CreateMeshes(GraphicsDevice device, float coneHeight, float coneRadius, float lineLength)
        {
            const int subdivisions = 10;

            int coneVertices = subdivisions + 2;
            int lineVertices = 2;
            int totalVertices = coneVertices + lineVertices;
            m_coneVerticesTotal = (uint)coneVertices;

            int coneIndices = (subdivisions * 2) * 3;
            int lineIndices = 2;
            int totalIndices = coneIndices + lineIndices;
            m_coneIndicesTotal = (uint)coneIndices;

            VertexPositionColor[] vertices = new VertexPositionColor[totalVertices];
            int[] triangles = new int[totalIndices];

            m_vertexBuffer = new VertexBuffer(device, VertexPositionColor.VertexDeclaration, totalVertices);
            m_indexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, totalIndices);

            CreateLine(vertices, triangles, lineLength, coneHeight, 0, 0);
            CreateCone(vertices, triangles, subdivisions, coneRadius, coneHeight, lineLength, 2, 2);

            m_primitiveCount = m_coneIndicesTotal / 3;

            m_vertexBuffer.SetData(vertices);
            m_indexBuffer.SetData(triangles);
        }

        private void CreateLine(VertexPositionColor[] vertices, int[] triangles, float lineLength, float coneHeight, int verticesOffset, int indicesOffset)
        {
            float heightOffset = -(coneHeight + lineLength) * 0.5f;

            vertices[verticesOffset] = new VertexPositionColor(new Vector3(0.0f, -heightOffset, 0.0f), m_color);
            vertices[verticesOffset + 1] = new VertexPositionColor(new Vector3(0.0f, heightOffset, 0.0f), m_color);

            triangles[indicesOffset] = verticesOffset;
            triangles[indicesOffset + 1] = verticesOffset + 1;
        }

        private void CreateCone(VertexPositionColor[] vertices, int[] triangles, int subdivisions, float radius, float height, float lineLength, int verticesOffset, int indicesOffset)
        {
            float heightOffset = (height + lineLength) * 0.5f;
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
            device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, 1);
            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 2, 2, PrimitiveCount);
        }
    }
}
