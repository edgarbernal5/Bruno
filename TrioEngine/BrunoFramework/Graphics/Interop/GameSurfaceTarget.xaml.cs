
using BrunoFramework.Editor;
using BrunoFramework.Editor.Graphics;
using BrunoWindows;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using BrunoApi.Net.Graphics;

namespace BrunoFramework.Graphics.Interop
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

        public IList<GameGraphicsScreen> GameGraphicsScreens
        {
            get { return (IList<GameGraphicsScreen>)GetValue(GameGraphicsScreensProperty); }
            set { SetValue(GameGraphicsScreensProperty, value); }
        }

        public static readonly DependencyProperty GameGraphicsScreensProperty = DependencyProperty.Register(
            "GameGraphicsScreens",
            typeof(IList<GameGraphicsScreen>),
            typeof(GameSurfaceTarget),
            new PropertyMetadata(new List<GameGraphicsScreen>(), OnGameGraphicsScreensChanged));

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

            Loaded += GameSurfaceTarget_Loaded;
            Unloaded += GameSurfaceTarget_Unloaded;
        }

        ~GameSurfaceTarget()
        {
            m_gameImageSource.Dispose();
        }

        private static void OnGameGraphicsScreensChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            //var target = (GameSurfaceTarget)dependencyObject;
            //var oldValue = (IEnumerable<GameGraphicsScreen>)eventArgs.OldValue;
            //var newValue = (IEnumerable<GameGraphicsScreen>)eventArgs.NewValue;
            //target.OnGraphicsScreensChanged(oldValue, newValue);
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
