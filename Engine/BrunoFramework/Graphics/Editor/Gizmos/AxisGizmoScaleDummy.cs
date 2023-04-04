using Bruno;
using System;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoScaleDummy : DisposableBase
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
        private uint m_boxIndicesTotal;

        public AxisGizmoScaleDummy(GraphicsDevice device, ColorRGBA8 color, float boxSize, float lineLength)
        {
            m_color = color;

            CreateMeshes(device, boxSize, lineLength);
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                m_indexBuffer.Dispose();
                m_vertexBuffer.Dispose();
            }
        }

        private void CreateMeshes(GraphicsDevice device, float boxSize, float lineLength)
        {
            int boxVertices = 8;
            int lineVertices = 2;
            int totalVertices = boxVertices + lineVertices;

            int boxIndices = 36;
            int lineIndices = 2;
            int totalIndices = boxIndices + lineIndices;
            m_boxIndicesTotal = (uint)boxIndices;

            VertexPositionColor[] vertices = new VertexPositionColor[totalVertices];
            int[] triangles = new int[totalIndices];

            m_vertexBuffer = new VertexBuffer(device, VertexPositionColor.VertexDeclaration, totalVertices);
            m_indexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, totalIndices);

            CreateLine(vertices, triangles, lineLength, boxSize, 0, 0);
            CreateBox(vertices, triangles, boxSize, lineLength, 2, 2);

            m_primitiveCount = m_boxIndicesTotal / 3;

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

        private void CreateBox(VertexPositionColor[] vertices, int[] triangles, float boxSize, float lineLength, int verticesOffset, int indicesOffset)
        {
            float heightOffset = (boxSize + lineLength) * 0.5f;
            var offset = new Vector3(0.0f, heightOffset, 0.0f);

            var verticesOffsets = new Vector3[8]
            {
                new Vector3(boxSize * 0.5f, boxSize * 0.5f, boxSize * 0.5f),
                new Vector3(-boxSize * 0.5f,  boxSize * 0.5f, boxSize * 0.5f),
                new Vector3(-boxSize * 0.5f,  boxSize * 0.5f, -boxSize * 0.5f),
                new Vector3(boxSize * 0.5f,  boxSize * 0.5f, -boxSize * 0.5f),

                new Vector3(boxSize * 0.5f, -boxSize * 0.5f, boxSize * 0.5f),
                new Vector3(-boxSize * 0.5f, -boxSize * 0.5f, boxSize * 0.5f),
                new Vector3(-boxSize * 0.5f, -boxSize * 0.5f, -boxSize * 0.5f),
                new Vector3(boxSize * 0.5f, -boxSize * 0.5f, -boxSize * 0.5f),
            };

            for (int i = 0; i < 8; i++)
            {
                vertices[verticesOffset + i] = new VertexPositionColor(offset + verticesOffsets[i], m_color);
            }

            //h tope
            triangles[indicesOffset] = 0;
            triangles[indicesOffset + 1] = 1;
            triangles[indicesOffset + 2] = 2;

            triangles[indicesOffset + 3] = 3;
            triangles[indicesOffset + 4] = 0;
            triangles[indicesOffset + 5] = 2;

            //h fondo
            indicesOffset += 6;
            triangles[indicesOffset] = 6;
            triangles[indicesOffset + 1] = 5;
            triangles[indicesOffset + 2] = 4;

            triangles[indicesOffset + 3] = 6;
            triangles[indicesOffset + 4] = 4;
            triangles[indicesOffset + 5] = 7;

            //F
            indicesOffset += 6;
            triangles[indicesOffset] = 7;
            triangles[indicesOffset + 1] = 3;
            triangles[indicesOffset + 2] = 2;

            triangles[indicesOffset + 3] = 2;
            triangles[indicesOffset + 4] = 6;
            triangles[indicesOffset + 5] = 7;

            //lf
            indicesOffset += 6;
            triangles[indicesOffset] = 5;
            triangles[indicesOffset + 1] = 6;
            triangles[indicesOffset + 2] = 2;

            triangles[indicesOffset + 3] = 2;
            triangles[indicesOffset + 4] = 1;
            triangles[indicesOffset + 5] = 5;

            //rb
            indicesOffset += 6;
            triangles[indicesOffset] = 7;
            triangles[indicesOffset + 1] = 4;
            triangles[indicesOffset + 2] = 0;

            triangles[indicesOffset + 3] = 0;
            triangles[indicesOffset + 4] = 3;
            triangles[indicesOffset + 5] = 7;

            //B
            indicesOffset += 6;
            triangles[indicesOffset] = 1;
            triangles[indicesOffset + 1] = 0;
            triangles[indicesOffset + 2] = 4;

            triangles[indicesOffset + 3] = 4;
            triangles[indicesOffset + 4] = 5;
            triangles[indicesOffset + 5] = 1;
        }

        public void Draw(GraphicsDevice device)
        {
            device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, 1);
            device.DrawIndexedPrimitives(PrimitiveType.TriangleList, 2, 2, PrimitiveCount);
        }
    }
}
