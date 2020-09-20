
using Estero.Interop;
using System.Collections.Generic;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class GridMesh : DisposableBase
    {
        private VertexBuffer m_vertexBuffer;
        private IndexBuffer m_indexBuffer;
        private int m_gridSize;
        private float m_gridExtends;

        private int m_totalVertices;
        private int m_totalIndices;

        private Effect m_effect;

        public GridMesh(GraphicsDevice device, int gridSize)
        {
            m_gridSize = gridSize;
            if (m_gridSize % 2 == 0) m_gridSize++;

            m_gridExtends = m_gridSize / 2;

            CreateIndexBuffer(device);
            CreateVertexBuffer(device);

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
        }

        private void CreateVertexBuffer(GraphicsDevice device)
        {            
            List<VertexPositionColor> vertices = new List<VertexPositionColor>();
            var colorWhiteVector4 = ColorRGBA8.White;
            int halfSize = m_gridSize / 2;

            for (int i = 0; i < m_gridSize; i++)
            {
                var positionFront = new Vector3((float)(i - halfSize) , 0.0f, -m_gridExtends);
                var positionBack = new Vector3((float)(i - halfSize) , 0.0f, m_gridExtends);

                vertices.Add(new VertexPositionColor(positionFront, colorWhiteVector4));
                vertices.Add(new VertexPositionColor(positionBack, colorWhiteVector4));
            }

            for (int i = 1; i < m_gridSize - 1; i++)
            {
                var positionFront = new Vector3(-m_gridExtends, 0.0f, (float)(i - halfSize));
                var positionBack = new Vector3(m_gridExtends, 0.0f, (float)(i - halfSize));

                vertices.Add(new VertexPositionColor(positionFront, colorWhiteVector4));
                vertices.Add(new VertexPositionColor(positionBack, colorWhiteVector4));
            }

            m_totalVertices = vertices.Count; 
            m_vertexBuffer = new VertexBuffer(device, VertexPositionColor.VertexDeclaration, vertices.Count);
            m_vertexBuffer.SetData(vertices.ToArray());
        }

        private void CreateIndexBuffer(GraphicsDevice device)
        {           
            var indices = new List<int>();
            for (int i = 0; i < m_gridSize; i++)
            {
                indices.Add((i * 2));
                indices.Add((i * 2 + 1));
            }
            int offset = m_gridSize * 2;
            for (int i = 0; i < m_gridSize - 2; i++)
            {
                indices.Add((offset + (i * 2)));
                indices.Add((offset + (i * 2) + 1));
            }

            indices.Add(0);
            indices.Add(m_gridSize * 2 - 2);

            indices.Add(1);
            indices.Add(m_gridSize * 2 - 1);

            m_totalIndices = indices.Count;
            m_indexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, indices.Count);
            m_indexBuffer.SetData(indices.ToArray());
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                m_vertexBuffer.Dispose();
                m_indexBuffer.Dispose();
                m_effect.Dispose();
            }

            m_vertexBuffer = null;
            m_indexBuffer = null;
            m_effect = null;
        }

        public void Render(RenderContext renderContext)
        {
            var device = renderContext.GraphicsDevice;

            device.DepthStencilState = DepthStencilState.None;
            ////device->SetBlendState(BlendState::Opaque);
            device.RasterizerState = RasterizerState.CullNone;
            //device.SetSamplerState(0, SamplerState::LinearWrap);

            device.SetVertexBuffer(m_vertexBuffer);
            device.SetIndexBuffer(m_indexBuffer);

            Matrix viewProjection;
            Matrix view = Matrix.CreateLookAt(new Vector3(5.0f, 15.0f, 5.0f), Vector3.Zero, Vector3.Up);
            Matrix projection = Matrix.CreatePerspectiveFieldOfView(1.5708f, renderContext.Viewport.AspectRatio, 0.5f, 100.0f);

            viewProjection = view * projection;

            m_effect.Parameters["gWorldViewProj"].SetValue(viewProjection);
            m_effect.Techniques[0].Passes[0].Apply();

            device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, m_totalVertices, 0, m_totalIndices / 2);
        }
    }
}
