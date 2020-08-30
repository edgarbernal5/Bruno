using EsteroFramework.Editor.Timing;
using EsteroFramework.Graphics;
using System;
using System.Windows.Media;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Editor.Units.Game
{
    public class GameUnit : EditorUnit
    {
        private GraphicsService m_graphicsService;
        private GameStepTimer m_gameStepTimer;

        public GameUnit()
        {

        }

        protected override void OnInitialize()
        {
            var presentationParameters = new PresentationParameters();
            presentationParameters.BackBufferWidth = 1;
            presentationParameters.BackBufferHeight = 1;
            presentationParameters.DeviceWindowHandle = IntPtr.Zero;

            var graphicsDevice = new GraphicsDevice(GraphicsAdapter.DefaultAdapter, presentationParameters);

            m_graphicsService = new GraphicsService(graphicsDevice);
            Editor.Services.RegisterInstance(typeof(IGraphicsService), null, m_graphicsService);
        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            m_gameStepTimer.Tick();
        }

        private void GameStepTimer_OnTick()
        {
            //Console.WriteLine("Ticking elapsed = {0}, total = {1}", m_gameStepTimer.ElapsedTime.TotalSeconds, m_gameStepTimer.TotalTime.TotalSeconds);
        }

        protected override void OnStartup()
        {
            m_gameStepTimer = new GameStepTimer();
            m_gameStepTimer.OnTimeChanged += GameStepTimer_OnTick;

            CompositionTarget.Rendering += CompositionTarget_Rendering;

            m_gameStepTimer.Start();
        }

        protected override void OnShutdown()
        {
            CompositionTarget.Rendering -= CompositionTarget_Rendering;
            m_gameStepTimer.Stop();
            m_gameStepTimer.OnTimeChanged -= GameStepTimer_OnTick;
        }

        protected override void OnUninitialize()
        {
            var device = m_graphicsService.GraphicsDevice;
            device.Dispose();

            Editor.Services.UnregisterHandler(typeof(IGraphicsService));
        }
    }
}
