using Bruno.Interop;
using BrunoFramework.Editor.Game.Gizmos;
using System.Collections.Generic;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class AxisGizmoScale : DisposableBase
    {
        private ColorRGBA8[] m_colors;
        private readonly float m_boxSize;
        private readonly float m_lineLength;

        public int PrimitiveCount
        {
            get { return m_primitiveCount; }
        }
        private int m_primitiveCount;

        private VertexPositionColor[] m_positionColorVertex;
        private short[] m_boxIndices;

        public AxisGizmoScale(GraphicsDevice device, ColorRGBA8[] colors, float boxSize, float lineLength)
        {
            m_colors = colors;
            m_boxSize = boxSize;
            m_lineLength = lineLength;
            CreateMeshes(device, boxSize, lineLength);
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {

            }
        }

        private void CreateMeshes(GraphicsDevice device, float boxSize, float lineLength)
        {
            m_boxIndices = CreateBoxIndices();

            var list = CreateBox(boxSize, lineLength);
            m_positionColorVertex = list.ToArray();

            m_primitiveCount = m_boxIndices.Length / 3;
        }

        public void PutOffsetInVertices(Vector3 deltaOffset, GizmoAxis gizmoAxis)
        {
            var axisIndex = gizmoAxis == GizmoAxis.X ? 0 : (gizmoAxis == GizmoAxis.Y ? 8 : 16);

            for (int i = axisIndex; i < axisIndex + 8; i++)
            {
                m_positionColorVertex[i].Position += deltaOffset;
            }
        }

        public void PutBackBox(GizmoAxis gizmoAxis)
        {
            var axisIndex = gizmoAxis == GizmoAxis.X ? 0 : (gizmoAxis == GizmoAxis.Y ? 8 : 16);
            var axisDirection = gizmoAxis == GizmoAxis.X ? Vector3.UnitX : (gizmoAxis == GizmoAxis.Y ? Vector3.UnitY : Vector3.UnitZ);

            var offset = axisDirection * m_lineLength;

            m_positionColorVertex[axisIndex].Position = new Vector3(m_boxSize * 0.5f, m_boxSize * 0.5f, m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 1].Position = new Vector3(-m_boxSize * 0.5f, m_boxSize * 0.5f, m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 2].Position = new Vector3(-m_boxSize * 0.5f, m_boxSize * 0.5f, -m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 3].Position = new Vector3(m_boxSize * 0.5f, m_boxSize * 0.5f, -m_boxSize * 0.5f) + offset;
            
            m_positionColorVertex[axisIndex + 4].Position = new Vector3(m_boxSize * 0.5f, -m_boxSize * 0.5f, m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 5].Position = new Vector3(-m_boxSize * 0.5f, -m_boxSize * 0.5f, m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 6].Position = new Vector3(-m_boxSize * 0.5f, -m_boxSize * 0.5f, -m_boxSize * 0.5f) + offset;
            m_positionColorVertex[axisIndex + 7].Position = new Vector3(m_boxSize * 0.5f, -m_boxSize * 0.5f, -m_boxSize * 0.5f) + offset;

        }

        private void FillVerticesBox(List<VertexPositionColor> list, ColorRGBA8 color, float boxSize, float lineLength, Vector3 offset)
        {
            list.Add(new VertexPositionColor(new Vector3(boxSize * 0.5f, boxSize * 0.5f, boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(-boxSize * 0.5f, boxSize * 0.5f, boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(-boxSize * 0.5f, boxSize * 0.5f, -boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(boxSize * 0.5f, boxSize * 0.5f, -boxSize * 0.5f) + offset, color));

            list.Add(new VertexPositionColor(new Vector3(boxSize * 0.5f, -boxSize * 0.5f, boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(-boxSize * 0.5f, -boxSize * 0.5f, boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(-boxSize * 0.5f, -boxSize * 0.5f, -boxSize * 0.5f) + offset, color));
            list.Add(new VertexPositionColor(new Vector3(boxSize * 0.5f, -boxSize * 0.5f, -boxSize * 0.5f) + offset, color));
        }

        private List<VertexPositionColor> CreateBox(float boxSize, float lineLength)
        {
            var list = new List<VertexPositionColor>();

            var heightOffset = lineLength;

            FillVerticesBox(list, m_colors[0], boxSize, lineLength, Vector3.UnitX * heightOffset);
            FillVerticesBox(list, m_colors[1], boxSize, lineLength, Vector3.UnitY * heightOffset);
            FillVerticesBox(list, m_colors[2], boxSize, lineLength, Vector3.UnitZ * heightOffset);

            return list;
        }

        private short[] CreateBoxIndices()
        {
            var array = new short[36*3];
            FillBoxIndices(array, 0, 0);
            FillBoxIndices(array, 36, 8);
            FillBoxIndices(array, 36 * 2, 16);

            return array;
        }

        private void FillBoxIndices(short[] triangles, int indicesOffset, int boxOffset)
        {
            //h tope
            triangles[indicesOffset] = (short)(0 + boxOffset);
            triangles[indicesOffset + 1] = (short)(1 + boxOffset);
            triangles[indicesOffset + 2] = (short)(2 + boxOffset);

            triangles[indicesOffset + 3] = (short)(3 + boxOffset);
            triangles[indicesOffset + 4] = (short)(0 + boxOffset);
            triangles[indicesOffset + 5] = (short)(2 + boxOffset);

            //h fondo
            indicesOffset += 6;
            triangles[indicesOffset] = (short)(6 + boxOffset);
            triangles[indicesOffset + 1] = (short)(5 + boxOffset);
            triangles[indicesOffset + 2] = (short)(4 + boxOffset);

            triangles[indicesOffset + 3] = (short)(6 + boxOffset);
            triangles[indicesOffset + 4] = (short)(4 + boxOffset);
            triangles[indicesOffset + 5] = (short)(7 + boxOffset);

            //F
            indicesOffset += 6;
            triangles[indicesOffset] = (short)(7 + boxOffset);
            triangles[indicesOffset + 1] = (short)(3 + boxOffset);
            triangles[indicesOffset + 2] = (short)(2 + boxOffset);

            triangles[indicesOffset + 3] = (short)(2 + boxOffset);
            triangles[indicesOffset + 4] = (short)(6 + boxOffset);
            triangles[indicesOffset + 5] = (short)(7 + boxOffset);

            //lf
            indicesOffset += 6;
            triangles[indicesOffset] = (short)(5 + boxOffset);
            triangles[indicesOffset + 1] = (short)(6 + boxOffset);
            triangles[indicesOffset + 2] = (short)(2 + boxOffset);

            triangles[indicesOffset + 3] = (short)(2 + boxOffset);
            triangles[indicesOffset + 4] = (short)(1 + boxOffset);
            triangles[indicesOffset + 5] = (short)(5 + boxOffset);

            //rb
            indicesOffset += 6;
            triangles[indicesOffset] = (short)(7 + boxOffset);
            triangles[indicesOffset + 1] = (short)(4 + boxOffset);
            triangles[indicesOffset + 2] = (short)(0 + boxOffset);

            triangles[indicesOffset + 3] = (short)(0 + boxOffset);
            triangles[indicesOffset + 4] = (short)(3 + boxOffset);
            triangles[indicesOffset + 5] = (short)(7 + boxOffset);

            //B
            indicesOffset += 6;
            triangles[indicesOffset] = (short)(1 + boxOffset);
            triangles[indicesOffset + 1] = (short)(0 + boxOffset);
            triangles[indicesOffset + 2] = (short)(4 + boxOffset);

            triangles[indicesOffset + 3] = (short)(4 + boxOffset);
            triangles[indicesOffset + 4] = (short)(5 + boxOffset);
            triangles[indicesOffset + 5] = (short)(1 + boxOffset);
        }

        public void Draw(GraphicsDevice device)
        {
            device.DrawUserIndexedPrimitives(PrimitiveType.TriangleList, 
                m_positionColorVertex, 0, m_positionColorVertex.Length, 
                m_boxIndices, 0, m_primitiveCount, VertexPositionColor.VertexDeclaration);
        }
    }
}

