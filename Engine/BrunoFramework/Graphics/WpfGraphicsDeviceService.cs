
using System;
using System.Collections.Generic;
using BrunoApi.Net.Graphics;

namespace BrunoFramework.Graphics
{
    public class WpfGraphicsDeviceService : IHwndHostRef
    {
        private GraphicsDevice m_graphicsDevice;
        private Dictionary<IntPtr, bool> m_isReadyByHwnd;

        public WpfGraphicsDeviceService(GraphicsDevice graphicsDevice)
        {
            m_isReadyByHwnd = new Dictionary<IntPtr, bool>();
            m_graphicsDevice = graphicsDevice;
        }

        public void AddRef(IntPtr windowHandle, int width, int height)
        {
            var parameters = m_graphicsDevice.PresentationParameters;

            parameters.BackBufferWidth = Math.Max(width, 1);
            parameters.BackBufferHeight = Math.Max(height, 1);
            parameters.DeviceWindowHandle = windowHandle;

            m_graphicsDevice.Reset(parameters);
            m_isReadyByHwnd[windowHandle] = true;
        }

        public void Reset(IntPtr windowHandle, int width, int height)
        {
            if (!m_isReadyByHwnd.ContainsKey(windowHandle)) return;

            var parameters = m_graphicsDevice.PresentationParameters;

            parameters.BackBufferWidth = Math.Max(width, 1);
            parameters.BackBufferHeight = Math.Max(height, 1);
            parameters.DeviceWindowHandle = windowHandle;

            m_graphicsDevice.Reset(parameters);
        }

        public void RemoveRef(IntPtr windowHandle)
        {

        }
    }
}
