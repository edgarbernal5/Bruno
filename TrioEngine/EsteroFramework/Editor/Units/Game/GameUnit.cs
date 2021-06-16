using EsteroFramework.Editor.Timing;
using EsteroFramework.Graphics;
using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using TrioApi.Net.Graphics;

namespace EsteroFramework.Editor.Units
{
    public class GameUnit : EditorUnit
    {
        private GraphicsService m_graphicsService;
        public static GameStepTimer m_gameStepTimer;
        private bool m_idle;

        public GameUnit()
        {
            Priority = int.MaxValue - 1;
        }

        protected override void OnInitialize()
        {
            var presentationParameters = new PresentationParameters();
            presentationParameters.BackBufferWidth = 1;
            presentationParameters.BackBufferHeight = 1;
            presentationParameters.DeviceWindowHandle = IntPtr.Zero;
            presentationParameters.DepthStencilFormat = DepthFormat.Depth24Stencil8;
            presentationParameters.BackBufferFormat = SurfaceFormat.Color;
            presentationParameters.PresentationInterval = PresentInterval.Immediate;
            presentationParameters.IsFullScreen = false;

            var graphicsDevice = new GraphicsDevice(GraphicsAdapter.DefaultAdapter, presentationParameters);

            m_graphicsService = new GraphicsService(graphicsDevice);
            Editor.Services.RegisterInstance(typeof(IGraphicsService), null, m_graphicsService);

            var graphicsDeviceService = new WpfGraphicsDeviceService(graphicsDevice);
            Editor.Services.RegisterInstance(typeof(IHwndHostRef), null, graphicsDeviceService);

            m_gameStepTimer = new GameStepTimer();
            Editor.Services.RegisterInstance(typeof(GameStepTimer), null, m_gameStepTimer);
        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            m_gameStepTimer.Tick();
            Render();
            m_idle = false;
        }

        private void GameStepTimer_OnTick()
        {
            //Console.WriteLine("Ticking elapsed = {0}, total = {1}: FPS = {2}", m_gameStepTimer.ElapsedTime.TotalSeconds, m_gameStepTimer.TotalTime.TotalSeconds, m_gameStepTimer.FramesPerSecond);
        }

        private void Render()
        {
            foreach (var surfaceTarget in m_graphicsService.GameSurfaceTargets)
            {
                var graphicsScreens = surfaceTarget.GameGraphicsScreens;

                var sizeWidth = surfaceTarget.Width;
                var sizeHeight = surfaceTarget.Height;

                var lastSizeWith = surfaceTarget.LastWidth;
                var lastSizeHeight = surfaceTarget.LastHeight;

                if ((sizeWidth != lastSizeWith || sizeHeight != lastSizeHeight) && !m_idle)
                    continue;

                surfaceTarget.LastWidth = sizeWidth;
                surfaceTarget.LastHeight = sizeHeight;

                m_graphicsService.Render(surfaceTarget, graphicsScreens);
            }
        }

        protected override void OnStartup()
        {
            m_gameStepTimer.OnTimeChanged += GameStepTimer_OnTick;

            CompositionTarget.Rendering += CompositionTarget_Rendering;
            ComponentDispatcher.ThreadIdle += OnApplicationIdle;

            m_gameStepTimer.Start();
        }

        private void OnApplicationIdle(object sender, EventArgs eventArgs)
        {
            m_idle = true;
        }

        protected override void OnShutdown()
        {
            m_gameStepTimer.Stop();

            CompositionTarget.Rendering -= CompositionTarget_Rendering;
            ComponentDispatcher.ThreadIdle -= OnApplicationIdle;

            m_gameStepTimer.OnTimeChanged -= GameStepTimer_OnTick;
        }

        protected override void OnUninitialize()
        {
            var device = m_graphicsService.GraphicsDevice;
            device.Dispose();

            Editor.Services.UnregisterHandler(typeof(IGraphicsService));
            Editor.Services.UnregisterHandler(typeof(IHwndHostRef));
        }
    }
}
