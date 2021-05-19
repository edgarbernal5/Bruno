
using System;
using TrioApi.Net.Graphics;

namespace EsteroFramework.Graphics
{
    public class WpfGraphicsDeviceService : IHwndHostRef
    {
        private GraphicsDevice m_graphicsDevice;

        public WpfGraphicsDeviceService(GraphicsDevice graphicsDevice)
        {
            m_graphicsDevice = graphicsDevice;
        }

        private bool m_isReady;

        public WpfGraphicsDeviceService()
        {
            m_isReady = false;
        }

        public void AddRef(IntPtr windowHandle, int width, int height)
        {
            var parameters = m_graphicsDevice.PresentationParameters;

            parameters.BackBufferWidth = Math.Max(width, 1);
            parameters.BackBufferHeight = Math.Max(height, 1);
            parameters.DeviceWindowHandle = windowHandle;

            m_graphicsDevice.Reset(parameters);
            m_isReady = true;
        }

        public void Reset(int width, int height)
        {
            if (!m_isReady) return;

            var parameters = m_graphicsDevice.PresentationParameters;

            parameters.BackBufferWidth = Math.Max(width, 1);
            parameters.BackBufferHeight = Math.Max(height, 1);

            m_graphicsDevice.Reset(parameters);
        }

        public void RemoveRef(IntPtr windowHandle)
        {

        }
    }
}
