using BrunoFramework.Editor.Timing;
using BrunoFramework.Graphics;
using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using BrunoApi.Net.Graphics;
using Bruno.Logging;

namespace BrunoFramework.Editor.Units
{
    public class GameUnit : EditorUnit, IGameUnitService
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        private GraphicsService m_graphicsService;
        public static GameStepTimer m_gameStepTimer;
        private bool m_idle;

        public event GameUnitEvent OnNewFrame;

        public GameUnit()
            : base()
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
            Editor.Services.RegisterInstance(typeof(IGraphicsService), m_graphicsService);

            var graphicsDeviceService = new WpfGraphicsDeviceService(graphicsDevice);
            Editor.Services.RegisterInstance(typeof(IHwndHostRef), graphicsDeviceService);

            m_gameStepTimer = new GameStepTimer();
            Editor.Services.RegisterInstance(typeof(GameStepTimer), m_gameStepTimer);

            Editor.Services.RegisterInstance(typeof(IGameUnitService), this);
        }

        private void OnCompositionTargetRendering(object sender, EventArgs e)
        {
            m_gameStepTimer.Tick();
            Render();
            m_idle = false;
        }

        private void OnGameStepTimerTick()
        {
            //Logger.Debug("Ticking elapsed = {0}, total = {1}: FPS = {2}", m_gameStepTimer.ElapsedTime.TotalSeconds, m_gameStepTimer.TotalTime.TotalSeconds, m_gameStepTimer.FramesPerSecond);
            OnNewFrame?.Invoke();
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
            m_gameStepTimer.OnTimeChanged += OnGameStepTimerTick;

            CompositionTarget.Rendering += OnCompositionTargetRendering;
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

            CompositionTarget.Rendering -= OnCompositionTargetRendering;
            ComponentDispatcher.ThreadIdle -= OnApplicationIdle;

            m_gameStepTimer.OnTimeChanged -= OnGameStepTimerTick;
        }

        protected override void OnUninitialize()
        {
            var device = m_graphicsService.GraphicsDevice;
            device.Dispose();

            Editor.Services.UnregisterHandler(typeof(IGraphicsService));
            Editor.Services.UnregisterHandler(typeof(IHwndHostRef));
            Editor.Services.UnregisterHandler(typeof(IGameUnitService));
        }
    }
}
