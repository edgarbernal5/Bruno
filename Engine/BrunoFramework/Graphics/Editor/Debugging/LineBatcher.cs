
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Editor
{
    public class LineBatcher
    {
        private const int BUFFER_SIZE = 1024;

        private VertexPositionColor[] m_buffer;

        private int m_lineCount;

        public LineBatcher(Effect effect)
        {
            m_lineCount = 0;
            m_buffer = new VertexPositionColor[BUFFER_SIZE];
        }

        public void Clear()
        {
            m_lineCount = 0;
        }

        public void Add(Vector3 startPoint, Vector3 endPoint, ColorRGBA8 color)
        {
            m_buffer[m_lineCount * 2 + 0].Position = startPoint;
            m_buffer[m_lineCount * 2 + 0].Color = color;
            m_buffer[m_lineCount * 2 + 1].Position = endPoint;
            m_buffer[m_lineCount * 2 + 1].Color = color;

            m_lineCount++;
        }

        public void Render(RenderContext renderContext)
        {
            if (m_lineCount <= 0) return;

            var device = renderContext.GraphicsDevice;

            device.DrawUserPrimitives(PrimitiveType.LineList, m_buffer, 0, m_lineCount);
        }
    }
}
