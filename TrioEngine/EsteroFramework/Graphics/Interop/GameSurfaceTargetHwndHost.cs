
using EsteroFramework.Editor;
using EsteroFramework.Editor.Graphics;
using EsteroWindows.Interop;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;
using TrioApi.Net.Graphics;

namespace EsteroFramework.Graphics.Interop
{
    public class GameSurfaceTargetHwndHost : HwndHost, IGameSurfaceTarget
    {
        private const string windowClass = "GameSurfaceTargetHwndHostWindowClass";

        private IntPtr m_hWnd;
        IGraphicsService m_graphicsService;

        public int LastWidth { get; set; }

        public int LastHeight { get; set; }

        public IList<GameGraphicsScreen> GameGraphicsScreens
        {
            get { return (IList<GameGraphicsScreen>)GetValue(GameGraphicsScreensProperty); }
            set { SetValue(GameGraphicsScreensProperty, value); }
        }

        public static readonly DependencyProperty GameGraphicsScreensProperty = DependencyProperty.Register(
            "GameGraphicsScreens",
            typeof(IList<GameGraphicsScreen>),
            typeof(GameSurfaceTargetHwndHost),
            new PropertyMetadata(new List<GameGraphicsScreen>(), OnGameGraphicsScreensChanged));

        public GameImageSource GameImageSource => throw new NotImplementedException();

        int IGameSurfaceTarget.Width => (int)ActualWidth;

        int IGameSurfaceTarget.Height => (int)ActualHeight;

        //public new int Width
        //{
        //    get
        //    {
        //        return (int)ActualWidth;
        //    }
        //}

        //public new int Height
        //{
        //    get
        //    {
        //        return (int)ActualHeight;
        //    }
        //}


        public GameSurfaceTargetHwndHost()
        {
            Initialize();
            Loaded += GameSurfaceTargetHost_Loaded;
            Unloaded += GameSurfaceTargetHost_Unloaded;
            SizeChanged += GameSurfaceTargetHwndHost_SizeChanged;
        }

        private void Initialize()
        {
            m_hWnd = IntPtr.Zero;
        }

        private void GameSurfaceTargetHost_Loaded(object sender, RoutedEventArgs e)
        {
            var editor = this.GetEditor();
            m_graphicsService = editor.Services.GetInstance<IGraphicsService>();
            var graphicsDeviceService = editor.Services.GetInstance<IAddHwndHostRef>();

            m_graphicsService.GameSurfaceTargets.Add(this);
            graphicsDeviceService.AddHostRef(m_hWnd, (int)ActualWidth, (int)ActualHeight);

            //GameGraphicsScreens = new List<GameGraphicsScreen>();
            //GameGraphicsScreens.Add(new EmptyGameGraphicsScreen());

            //var editor = this.GetEditor();
            //var presentationParameters = new PresentationParameters();
            //presentationParameters.BackBufferWidth = (int)ActualWidth;
            //presentationParameters.BackBufferHeight = (int)ActualHeight;
            //presentationParameters.DeviceWindowHandle = m_hWnd;
            //presentationParameters.DepthStencilFormat = DepthFormat.Depth24Stencil8;
            //presentationParameters.BackBufferFormat = SurfaceFormat.Color;
            //presentationParameters.PresentationInterval = PresentInterval.Immediate;
            //presentationParameters.IsFullScreen = false;

            //var graphicsDevice = new GraphicsDevice(GraphicsAdapter.DefaultAdapter, presentationParameters);

            //m_graphicsService = new GraphicsService(graphicsDevice);
            //editor.Services.RegisterInstance(typeof(IGraphicsService), null, m_graphicsService);

            //var graphicsDeviceService = new WpfGraphicsDeviceService(graphicsDevice);
            //editor.Services.RegisterInstance(typeof(IAddHwndHostRef), null, graphicsDeviceService);
            //m_graphicsService.GameSurfaceTargets.Add(this);
        }


        private void GameSurfaceTargetHost_Unloaded(object sender, RoutedEventArgs e)
        {
            m_graphicsService.GameSurfaceTargets.Remove(this);
        }

        private void GameSurfaceTargetHwndHost_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var editor = this.GetEditor();
            var graphicsDeviceService = editor.Services.GetInstance<IAddHwndHostRef>();
            
            if (graphicsDeviceService != null)
            {
                graphicsDeviceService.Reset((int)ActualWidth, (int)ActualHeight);
            }
        }

        private static void OnGameGraphicsScreensChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
        }

        public bool BeginRender(RenderContext context)
        {
            context.Viewport = new Viewport(0, 0, (int)ActualWidth, (int)ActualHeight);

            return true;
        }

        public void EndRender()
        {
            m_graphicsService.GraphicsDevice.Present();
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            // Create the host window as a child of the parent
            m_hWnd = CreateHostWindow(hwndParent.Handle);
            return new HandleRef(this, m_hWnd);
        }

        private IntPtr CreateHostWindow(IntPtr hWndParent)
        {
            // Register our window class
            RegisterWindowClass();

            // Create the window
            return Win32.CreateWindowEx(0, windowClass, "",
               (int)(WindowStyles.WS_CHILD | WindowStyles.WS_VISIBLE),
               0, 0, (int)Width, (int)Height, hWndParent, IntPtr.Zero, IntPtr.Zero, 0);
        }

        private void RegisterWindowClass()
        {
            WNDCLASSEX wndClass = new WNDCLASSEX();
            wndClass.cbSize = (uint)Marshal.SizeOf(wndClass);
            wndClass.hInstance = Win32.GetModuleHandle(null);
            wndClass.lpfnWndProc = Win32.DefaultWindowProc;
            wndClass.lpszClassName = windowClass;
            wndClass.hCursor = Win32.LoadCursor(IntPtr.Zero, (int)LoadCursorNames.IDC_ARROW);

            Win32.RegisterClassEx(ref wndClass);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            // Destroy the window and reset our hWnd value
            Win32.DestroyWindow(hwnd.Handle);
            m_hWnd = IntPtr.Zero;
        }

        //protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        //{
        //    switch (msg)
        //    {
        //        case WindowMessages.WM_MOUSEWHEEL:
        //        case WindowMessages.WM_LBUTTONDOWN:
        //        case WindowMessages.WM_LBUTTONUP:
        //        case WindowMessages.WM_LBUTTONDBLCLK:
        //        case WindowMessages.WM_RBUTTONDOWN:
        //        case WindowMessages.WM_RBUTTONUP:
        //        case WindowMessages.WM_RBUTTONDBLCLK:
        //        case WindowMessages.WM_MBUTTONDOWN:
        //        case WindowMessages.WM_MBUTTONUP:
        //        case WindowMessages.WM_MBUTTONDBLCLK:
        //        case WindowMessages.WM_XBUTTONDOWN:
        //        case WindowMessages.WM_XBUTTONUP:
        //        case WindowMessages.WM_XBUTTONDBLCLK:
        //        case WindowMessages.WM_MOUSEMOVE:
        //        case WindowMessages.WM_MOUSELEAVE:
        //            handled = false;
        //            //return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        //            return IntPtr.Zero;
        //    }
        //    return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        //}
    }
}