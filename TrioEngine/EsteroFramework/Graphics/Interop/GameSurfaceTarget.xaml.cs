
using EsteroFramework.Editor;
using EsteroWindows;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using TrioWpfFramework.Net.Graphics;

namespace EsteroFramework.Graphics.Interop
{
    public partial class GameSurfaceTarget : UserControl, IGameSurfaceTarget
    {
        public IntPtr Handle
        {
            get
            {
                //(PresentationSource.FromVisual(this) as HwndSource).Handle
                //HwndSource source = (HwndSource)HwndSource.FromVisual(this);
                //source.Handle;
                return (PresentationSource.FromVisual(this) as HwndSource).Handle;
            }
        }

        public new int Width
        {
            get
            {
                return (int)ActualWidth;
            }
        }

        public new int Height
        {
            get
            {
                return (int)ActualHeight;
            }
        }

        public List<GameGraphicsScreen> GameGraphicsScreens
        {
            get
            {
                return m_gameGraphicsScreens;
            }
        }
        private List<GameGraphicsScreen> m_gameGraphicsScreens;

        public GameImageSource GameImageSource => m_gameImageSource;
        private GameImageSource m_gameImageSource;

        public int LastWidth { get; set; }

        public int LastHeight { get; set; }


        private IGraphicsService m_graphicsService;
        private GraphicsDevice m_device;

        public GameSurfaceTarget()
        {
            InitializeComponent();

            if (WindowsPlatform.InDesignMode) return;

            m_gameGraphicsScreens = new List<GameGraphicsScreen>();
            Loaded += GameSurfaceTarget_Loaded;
            Unloaded += GameSurfaceTarget_Unloaded;

            m_gameGraphicsScreens.Add(new BasicGraphicsScreen());
        }

        ~GameSurfaceTarget()
        {
            m_gameImageSource.Dispose();
        }

        private void GameSurfaceTarget_Loaded(object sender, RoutedEventArgs e)
        {
            if (WindowsPlatform.InDesignMode) return;

            var editor = this.GetEditor();
            m_graphicsService = editor.Services.GetInstance<IGraphicsService>();
            m_device = m_graphicsService.GraphicsDevice;

            m_gameImageSource = new GameImageSource(m_device, Width, Height);
            rootImage.Source = m_gameImageSource.WriteableBitmap;

            m_graphicsService.GameSurfaceTargets.Add(this);
        }

        private void GameSurfaceTarget_Unloaded(object sender, RoutedEventArgs e)
        {
            m_graphicsService.GameSurfaceTargets.Remove(this);
        }

        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            if (!WindowsPlatform.InDesignMode && m_graphicsService != null)
            {
                m_gameImageSource.Dispose();
                m_gameImageSource = new GameImageSource(m_device, Width, Height);

                rootImage.Source = m_gameImageSource.WriteableBitmap;
            }
            base.OnRenderSizeChanged(sizeInfo);
        }

        public bool BeginRender(RenderContext context)
        {
            var renderTarget = m_gameImageSource.RenderTarget;
            if (renderTarget == null)
                return false;

            m_device.SetRenderTarget(renderTarget);
            context.RenderTarget = renderTarget;
            context.Viewport = new Viewport(0, 0, renderTarget.Width, renderTarget.Height);

            return true;
        }

        public void EndRender()
        {
            m_device.SetRenderTarget(null);

            m_gameImageSource.Commit();
            //m_device.Flush();
        }
    }
}
