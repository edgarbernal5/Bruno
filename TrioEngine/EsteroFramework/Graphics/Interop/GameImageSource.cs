
using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using TrioApi.Net.Graphics;
using TrioApi.Net.Graphics.Core;

namespace EsteroFramework.Graphics.Interop
{
    public class GameImageSource : IDisposable
    {
        private byte[] m_buffer;

        public RenderTarget2D RenderTarget
        {
            get { return m_renderTarget; }
        }
        private RenderTarget2D m_renderTarget;

        public WriteableBitmap WriteableBitmap
        {
            get { return m_writeableBitmap; }
        }
        private WriteableBitmap m_writeableBitmap;

        public GameImageSource(GraphicsDevice graphicsDevice, int width, int height)
        {
            m_renderTarget = new RenderTarget2D
                (
                    graphicsDevice, width, height, 0,
                    SurfaceFormat.Color,
                    DepthFormat.Depth24Stencil8
                );

            m_buffer = new byte[width * height * 4];
            m_writeableBitmap = new WriteableBitmap
                (
                    width, height, 96, 96,
                    PixelFormats.Bgra32, null
                );
        }

        ~GameImageSource()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            m_renderTarget.Dispose();

            if (disposing)
                GC.SuppressFinalize(this);
        }

        public void Commit()
        {
            m_renderTarget.GetData(m_buffer);

            // because the only 32 bit pixel format for WPF is 
            // BGRA but XNA is all RGBA, we have to swap the R 
            // and B bytes for each pixel
            for (int i = 0; i < m_buffer.Length - 2; i += 4)
            {
                byte r = m_buffer[i];
                m_buffer[i] = m_buffer[i + 2];
                m_buffer[i + 2] = r;
            }

            // write our pixels into the bitmap source
            m_writeableBitmap.Lock();
            Marshal.Copy(m_buffer, 0, m_writeableBitmap.BackBuffer, m_buffer.Length);
            m_writeableBitmap.AddDirtyRect(
                new Int32Rect(0, 0, m_renderTarget.Width, m_renderTarget.Height));
            m_writeableBitmap.Unlock();
        }
    }
}
