using System;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroFramework.Graphics.Editor
{
    public class GridMesh : IDisposable
    {
        private bool m_disposed;

        private VertexBuffer m_vertexBuffer;
        private IndexBuffer m_indexBuffer;
        private int m_gridSize;

        private Effect m_effect;

        public GridMesh(GraphicsDevice device, int gridSize)
        {
            m_gridSize = gridSize;

            CreateIndexBuffer(device);
            CreateVertexBuffer(device);

            m_effect = new Effect(device, @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\Shaders\LineEffect.fx");
        }


        private void CreateVertexBuffer(GraphicsDevice device)
        {
            //int totalVertices = m_gridSize * 4 - 4;
            int totalVertices = m_gridSize * 2;

            m_vertexBuffer = new VertexBuffer(device, VertexPositionColor.VertexDeclaration, totalVertices);

            VertexPositionColor[] vertices = new VertexPositionColor[totalVertices];
            var colorWhiteVector4 = ColorRGBA8.White;
            for (int i = 0; i < m_gridSize; i++)
            {
                var positionFront = new Vector3(i - m_gridSize * 0.5f, 0.0f, -m_gridSize * 0.5f);
                var positionBack = new Vector3(i - m_gridSize * 0.5f, 0.0f, m_gridSize * 0.5f);

                vertices[i * 2] = new VertexPositionColor(positionFront, colorWhiteVector4);
                vertices[i * 2 + 1] = new VertexPositionColor(positionBack, colorWhiteVector4);
            }

            m_vertexBuffer.SetData(vertices);
        }

        private void CreateIndexBuffer(GraphicsDevice device)
        {
            int totalIndices = m_gridSize * 2;
            m_indexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, totalIndices);

            var indices = new int[totalIndices];
            for (int i = 0; i < m_gridSize; i++)
            {
                indices[i * 2] = (i * 2);
                indices[i * 2 + 1] = (i * 2 + 1);
            }

            m_indexBuffer.SetData(indices);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (!m_disposed)
                {
                    m_vertexBuffer.Dispose();
                    m_indexBuffer.Dispose();
                    m_disposed = true;
                }
            }
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
            Matrix view = Matrix.CreateLookAt(new Vector3(5.0f, 5.0f, 5.0f), Vector3.Zero, Vector3.Up);
            Matrix projection = Matrix.CreatePerspectiveFieldOfView(1.5708f, renderContext.Viewport.AspectRatio, 0.5f, 100.0f);

            viewProjection = view * projection;

            m_effect.Parameters["gWorldViewProj"].SetValue(viewProjection);
            m_effect.Techniques[0].Passes[0].Apply();

            device.DrawIndexedPrimitives(PrimitiveType.LineList, 0, 0, m_gridSize * 2, 0, m_gridSize);
        }
    }
}
