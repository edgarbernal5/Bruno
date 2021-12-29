
using BrunoFramework.Editor;
using BrunoFramework.Editor.Graphics;
using BrunoWindows.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using BrunoApi.Net.Graphics;

namespace BrunoFramework.Graphics.Interop
{
    public class GameSurfaceTargetHwndHost : HwndHost, IGameSurfaceTarget
    {
        private const string WINDOWS_CLASS = "GameSurfaceTargetHwndHostWindowClass";

        private IntPtr m_hWnd;
        private IntPtr m_hWndPrev;
        private bool m_applicationHasFocus;
        private bool m_mouseInWindow;
        private Point m_previousMousePosition;
        private bool m_isMouseCaptured;
        private readonly HwndMouseState m_mouseState = new HwndMouseState();

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
            new PropertyMetadata(null, OnGameGraphicsScreensChanged));

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

        public event EventHandler<HwndMouseEventArgs> HwndLButtonDown;

        public event EventHandler<HwndMouseEventArgs> HwndLButtonUp;

        public event EventHandler<HwndMouseEventArgs> HwndLButtonDblClick;

        public event EventHandler<HwndMouseEventArgs> HwndRButtonDown;

        public event EventHandler<HwndMouseEventArgs> HwndRButtonUp;

        public event EventHandler<HwndMouseEventArgs> HwndRButtonDblClick;

        public event EventHandler<HwndMouseEventArgs> HwndMButtonDown;

        public event EventHandler<HwndMouseEventArgs> HwndMButtonUp;

        public event EventHandler<HwndMouseEventArgs> HwndMButtonDblClick;

        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonDown;

        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonUp;

        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonDblClick;

        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonDown;

        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonUp;

        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonDblClick;

        public event EventHandler<HwndMouseEventArgs> HwndMouseMove;

        public event EventHandler<HwndMouseEventArgs> HwndMouseEnter;

        public event EventHandler<HwndMouseEventArgs> HwndMouseLeave;

        public event EventHandler<HwndMouseEventArgs> HwndMouseWheel;

        public new bool IsMouseCaptured
        {
            get { return m_isMouseCaptured; }
        }

        public GameSurfaceTargetHwndHost()
        {
            Initialize();
            HookEvents();
        }

        private void HookEvents()
        {
            Application.Current.Activated += Current_Activated;
            Application.Current.Deactivated += Current_Deactivated;

            Loaded += GameSurfaceTargetHost_Loaded;
            Unloaded += GameSurfaceTargetHost_Unloaded;
            SizeChanged += GameSurfaceTargetHwndHost_SizeChanged;
        }
        private IEditorService m_editorService;

        private void Initialize()
        {
            m_hWnd = IntPtr.Zero;

            if (Application.Current.Windows.Cast<Window>().Any(x => x.IsActive))
                m_applicationHasFocus = true;
        }

        private void Current_Activated(object sender, EventArgs e)
        {
            m_applicationHasFocus = true;
        }

        private void Current_Deactivated(object sender, EventArgs e)
        {
            m_applicationHasFocus = false;
            ResetMouseState();

            if (m_mouseInWindow)
            {
                m_mouseInWindow = false;
                HwndMouseLeave?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
            }

            ReleaseMouseCapture();
        }

        private void ResetMouseState()
        {
            // We need to invoke events for any buttons that were pressed
            bool fireL = m_mouseState.LeftButton == MouseButtonState.Pressed;
            bool fireM = m_mouseState.MiddleButton == MouseButtonState.Pressed;
            bool fireR = m_mouseState.RightButton == MouseButtonState.Pressed;
            bool fireX1 = m_mouseState.X1Button == MouseButtonState.Pressed;
            bool fireX2 = m_mouseState.X2Button == MouseButtonState.Pressed;

            // Update the state of all of the buttons
            m_mouseState.LeftButton = MouseButtonState.Released;
            m_mouseState.MiddleButton = MouseButtonState.Released;
            m_mouseState.RightButton = MouseButtonState.Released;
            m_mouseState.X1Button = MouseButtonState.Released;
            m_mouseState.X2Button = MouseButtonState.Released;

            // Fire any events
            var args = new HwndMouseEventArgs(m_mouseState);
            if (fireL)
                HwndLButtonUp?.Invoke(this, args);
            if (fireM)
                HwndMButtonUp?.Invoke(this, args);
            if (fireR)
                HwndRButtonUp?.Invoke(this, args);
            if (fireX1)
                HwndX1ButtonUp?.Invoke(this, args);
            if (fireX2)
                HwndX2ButtonUp?.Invoke(this, args);

            // The mouse is no longer considered to be in our window
            m_mouseInWindow = false;
        }

        private void GameSurfaceTargetHost_Loaded(object sender, RoutedEventArgs e)
        {
            var editor = this.GetEditor();
            m_editorService = editor; //HACK: REMOVE THIS!!

            var graphicsService = editor.Services.GetInstance<IGraphicsService>();
            if (graphicsService == null) return;
            var graphicsDeviceService = editor.Services.GetInstance<IHwndHostRef>();

            graphicsService.GameSurfaceTargets.Add(this);
            graphicsDeviceService.AddRef(m_hWnd, (int)ActualWidth, (int)ActualHeight);
        }

        private void GameSurfaceTargetHost_Unloaded(object sender, RoutedEventArgs e)
        {
            var editor = this.GetEditor() ?? m_editorService; //HACK: REMOVE THIS!!
            
            var graphicsService = editor.Services.GetInstance<IGraphicsService>();
            var graphicsDeviceService = editor.Services.GetInstance<IHwndHostRef>();

            graphicsService.GameSurfaceTargets.Remove(this);
            graphicsDeviceService.RemoveRef(m_hWnd);
        }

        private void GameSurfaceTargetHwndHost_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var editor = this.GetEditor();
            var graphicsDeviceService = editor.Services.GetInstance<IHwndHostRef>();
            
            if (graphicsDeviceService != null)
            {
                graphicsDeviceService.Reset(m_hWnd, (int)ActualWidth, (int)ActualHeight);
            }
        }

        private static void OnGameGraphicsScreensChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var target = (GameSurfaceTargetHwndHost)dependencyObject;
            var oldValue = (IList<GameGraphicsScreen>)eventArgs.OldValue;
            var newValue = (IList<GameGraphicsScreen>)eventArgs.NewValue;
            target.OnGameGraphicsScreensChanged(oldValue, newValue);
        }

        protected virtual void OnGameGraphicsScreensChanged(IList<GameGraphicsScreen> oldValue, IList<GameGraphicsScreen> newValue)
        {
        }

        public bool BeginRender(RenderContext context)
        {
            context.GraphicsDevice.PrepareRenderWindow(m_hWnd);
            context.Viewport = new Viewport(0, 0, (int)ActualWidth, (int)ActualHeight);

            return true;
        }

        public void EndRender()
        {
            var editor = this.GetEditor();
            var graphicsService = editor.Services.GetInstance<IGraphicsService>();
            graphicsService.GraphicsDevice.Present();
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
            return Win32.CreateWindowEx(0, WINDOWS_CLASS, "",
               (int)(WindowStyles.WS_CHILD | WindowStyles.WS_VISIBLE),
               0, 0, (int)Width, (int)Height, hWndParent, IntPtr.Zero, IntPtr.Zero, 0);
        }

        private void RegisterWindowClass()
        {
            WNDCLASSEX wndClass = new WNDCLASSEX();
            wndClass.cbSize = (uint)Marshal.SizeOf(wndClass);
            wndClass.hInstance = Win32.GetModuleHandle(null);
            wndClass.lpfnWndProc = Win32.DefaultWindowProc;
            wndClass.lpszClassName = WINDOWS_CLASS;
            wndClass.hCursor = Win32.LoadCursor(IntPtr.Zero, (int)LoadCursorNames.IDC_ARROW);

            Win32.RegisterClassEx(ref wndClass);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            // Destroy the window and reset our hWnd value
            Win32.DestroyWindow(hwnd.Handle);
            m_hWnd = IntPtr.Zero;
        }

        protected override void Dispose(bool disposing)
        {
            if (Application.Current != null)
            {
                Application.Current.Activated -= Current_Activated;
                Application.Current.Deactivated -= Current_Deactivated;
            }

            base.Dispose(disposing);
        }

        public new void CaptureMouse()
        {
            // Don't do anything if the mouse is already captured
            if (m_isMouseCaptured)
                return;

            Win32.SetCapture(m_hWnd);
            m_isMouseCaptured = true;
        }

        public new void ReleaseMouseCapture()
        {
            // Don't do anything if the mouse is not captured
            if (!m_isMouseCaptured)
                return;

            Win32.ReleaseCapture();
            m_isMouseCaptured = false;
        }

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case WindowMessages.WM_MOUSEWHEEL:
                    if (m_mouseInWindow)
                    {
                        int delta = Win32.GetWheelDeltaWParam(wParam);
                        HwndMouseWheel?.Invoke(this, new HwndMouseEventArgs(m_mouseState, delta, 0));
                    }
                    break;
                case WindowMessages.WM_LBUTTONDOWN:
                    m_mouseState.LeftButton = MouseButtonState.Pressed;
                    HwndLButtonDown?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_LBUTTONUP:
                    m_mouseState.LeftButton = MouseButtonState.Released;
                    HwndLButtonUp?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_LBUTTONDBLCLK:
                    HwndLButtonDblClick?.Invoke(this, new HwndMouseEventArgs(m_mouseState, MouseButton.Left));
                    break;
                case WindowMessages.WM_RBUTTONDOWN:
                    m_mouseState.RightButton = MouseButtonState.Pressed;
                    HwndRButtonDown?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_RBUTTONUP:
                    m_mouseState.RightButton = MouseButtonState.Released;
                    HwndRButtonUp?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_RBUTTONDBLCLK:
                    HwndRButtonDblClick?.Invoke(this, new HwndMouseEventArgs(m_mouseState, MouseButton.Right));
                    break;
                case WindowMessages.WM_MBUTTONDOWN:
                    m_mouseState.MiddleButton = MouseButtonState.Pressed;
                    HwndMButtonDown?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_MBUTTONUP:
                    m_mouseState.MiddleButton = MouseButtonState.Released;
                    HwndMButtonUp?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    break;
                case WindowMessages.WM_MBUTTONDBLCLK:
                    HwndMButtonDblClick?.Invoke(this, new HwndMouseEventArgs(m_mouseState, MouseButton.Middle));
                    break;
                case WindowMessages.WM_XBUTTONDOWN:
                    if (((int)wParam & WindowMessages.MK_XBUTTON1) != 0)
                    {
                        m_mouseState.X1Button = MouseButtonState.Pressed;
                        HwndX1ButtonDown?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    }
                    else if (((int)wParam & WindowMessages.MK_XBUTTON2) != 0)
                    {
                        m_mouseState.X2Button = MouseButtonState.Pressed;
                        HwndX2ButtonDown?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    }
                    break;
                case WindowMessages.WM_XBUTTONUP:
                    if (((int)wParam & WindowMessages.MK_XBUTTON1) != 0)
                    {
                        m_mouseState.X1Button = MouseButtonState.Released;
                        HwndX1ButtonUp?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    }
                    else if (((int)wParam & WindowMessages.MK_XBUTTON2) != 0)
                    {
                        m_mouseState.X2Button = MouseButtonState.Released;
                        HwndX2ButtonUp?.Invoke(this, new HwndMouseEventArgs(m_mouseState));
                    }
                    break;
                case WindowMessages.WM_XBUTTONDBLCLK:
                    if (((int)wParam & WindowMessages.MK_XBUTTON1) != 0)
                        HwndX1ButtonDblClick?.Invoke(this, new HwndMouseEventArgs(m_mouseState, MouseButton.XButton1));
                    else if (((int)wParam & WindowMessages.MK_XBUTTON2) != 0)
                        HwndX2ButtonDblClick?.Invoke(this, new HwndMouseEventArgs(m_mouseState, MouseButton.XButton2));
                    break;
                case WindowMessages.WM_MOUSEMOVE:
                    // If the application isn't in focus, we don't handle this message
                    if (!m_applicationHasFocus)
                        break;

                    // record the prevous and new position of the mouse
                    m_mouseState.ScreenPosition = PointToScreen(new Point(
                        Win32.GET_X_LPARAM(lParam),
                        Win32.GET_Y_LPARAM(lParam)));

                    if (!m_mouseInWindow)
                    {
                        m_mouseInWindow = true;

                        HwndMouseEnter?.Invoke(this, new HwndMouseEventArgs(m_mouseState));

                        // Track the previously focused window, and set focus to this window.
                        //m_hWndPrev = Win32.GetFocus();
                        //Win32.SetFocus(m_hWnd);

                        // send the track mouse event so that we get the WM_MOUSELEAVE message
                        var tme = new TRACKMOUSEEVENT
                        {
                            cbSize = Marshal.SizeOf(typeof(TRACKMOUSEEVENT)),
                            dwFlags = Win32.TME_LEAVE,
                            hWnd = hwnd
                        };
                        Win32.TrackMouseEvent(ref tme);
                    }

                    if (m_mouseState.ScreenPosition != m_previousMousePosition)
                        HwndMouseMove?.Invoke(this, new HwndMouseEventArgs(m_mouseState));

                    m_previousMousePosition = m_mouseState.ScreenPosition;

                    break;
                case WindowMessages.WM_MOUSELEAVE:

                    // If we have capture, we ignore this message because we're just
                    // going to reset the cursor position back into the window
                    if (m_isMouseCaptured)
                        break;

                    // Reset the state which releases all buttons and 
                    // marks the mouse as not being in the window.
                    ResetMouseState();

                    HwndMouseLeave?.Invoke(this, new HwndMouseEventArgs(m_mouseState));

                    //Win32.SetFocus(m_hWndPrev);

                    break;
            }

            return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        }
    }
}