using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using TrioApi.Net.Graphics;

namespace TrioWpfEditor.Delivery.Host
{
    public class GraphicsDeviceHost : HwndHost
    {
        private const string windowClass = "GraphicsDeviceHostWindowClass";

        // The HWND we present to when rendering
        private IntPtr hWnd;

        // For holding previous hWnd focus
        private IntPtr hWndprev;

        private GraphicsDeviceService graphicsService;

        private bool applicationHasFocus;
        private bool mouseInWindow;
        private bool isMouseCaptured;
        private int capturedMouseX;
        private int capturedMouseY;
        private int capturedMouseClientX;
        private int capturedMouseClientY;

        private HwndMouseState mouseState;

        /// <summary>
        /// Invoked when the control receives a left mouse down message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndLButtonDown;

        /// <summary>
        /// Invoked when the control receives a left mouse up message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndLButtonUp;

        /// <summary>
        /// Invoked when the control receives a left mouse double click message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndLButtonDblClick;

        /// <summary>
        /// Invoked when the control receives a right mouse down message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndRButtonDown;

        /// <summary>
        /// Invoked when the control receives a right mouse up message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndRButtonUp;

        /// <summary>
        /// Invoked when the control receives a rigt mouse double click message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndRButtonDblClick;

        /// <summary>
        /// Invoked when the control receives a middle mouse down message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMButtonDown;

        /// <summary>
        /// Invoked when the control receives a middle mouse up message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMButtonUp;

        /// <summary>
        /// Invoked when the control receives a middle mouse double click message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMButtonDblClick;

        /// <summary>
        /// Invoked when the control receives a mouse down message for the first extra button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonDown;

        /// <summary>
        /// Invoked when the control receives a mouse up message for the first extra button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonUp;

        /// <summary>
        /// Invoked when the control receives a double click message for the first extra mouse button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX1ButtonDblClick;

        /// <summary>
        /// Invoked when the control receives a mouse down message for the second extra button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonDown;

        /// <summary>
        /// Invoked when the control receives a mouse up message for the second extra button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonUp;

        /// <summary>
        /// Invoked when the control receives a double click message for the first extra mouse button.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndX2ButtonDblClick;

        /// <summary>
        /// Invoked when the control receives a mouse move message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMouseMove;

        /// <summary>
        /// Invoked when the control first gets a mouse move message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMouseEnter;

        /// <summary>
        /// Invoked when the control gets a mouse leave message.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMouseLeave;

        /// <summary>
        /// Invoked when the control recieves a mouse wheel delta.
        /// </summary>
        public event EventHandler<HwndMouseEventArgs> HwndMouseWheel;

        public GraphicsDeviceHost()
        {
            Initialize();
            Loaded += GraphicsDeviceHost_Loaded;
            SizeChanged += GraphicsDeviceHost_SizeChanged;

            Application.Current.Activated += Current_Activated;
            Application.Current.Deactivated += Current_Deactivated;

            // We use the CompositionTarget.Rendering event to trigger the control to draw itself
            CompositionTarget.Rendering += CompositionTarget_Rendering;
        }

        private void Initialize()
        {
            hWnd = IntPtr.Zero;
            applicationHasFocus = false;
            mouseInWindow = false;
            isMouseCaptured = false;
            mouseState = new HwndMouseState();
        }

        protected override void Dispose(bool disposing)
        {
            if (graphicsService != null)
            {
                graphicsService.Release(disposing);
                graphicsService = null;
            }

            CompositionTarget.Rendering -= CompositionTarget_Rendering;
            base.Dispose(disposing);
        }

        public new void CaptureMouse()
        {
            if (isMouseCaptured)
                return;

            NativeMethods.ShowCursor(false);
            isMouseCaptured = true;

            NativeMethods.POINT p = new NativeMethods.POINT();
            NativeMethods.GetCursorPos(ref p);
            capturedMouseX = p.X;
            capturedMouseY = p.Y;

            NativeMethods.ScreenToClient(hWnd, ref p);
            capturedMouseClientX = p.X;
            capturedMouseClientY = p.Y;
        }

        public new void ReleaseMouseCapture()
        {
            if (!isMouseCaptured)
                return;

            NativeMethods.ShowCursor(true);
            isMouseCaptured = false;
        }

        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            if (isMouseCaptured &&
                (int)mouseState.Position.X != capturedMouseX &&
                (int)mouseState.Position.Y != capturedMouseY)
            {
                NativeMethods.SetCursorPos(capturedMouseX, capturedMouseY);
                mouseState.Position = mouseState.PreviousPosition = new Point(capturedMouseClientX, capturedMouseClientY);
            }

            if (graphicsService == null) return;

            int width = (int)ActualWidth;
            int height = (int)ActualHeight;

            if (width < 1 || height < 1) return;

            Viewport viewport = new Viewport(0, 0, width, height);
            graphicsService.GraphicsDevice.Viewport = viewport;

            //graphicsService.GraphicsDevice.Present(viewport.Bounds, null, hWnd);
            graphicsService.GraphicsDevice.Present();
        }

        private void Current_Activated(object sender, EventArgs e)
        {
            applicationHasFocus = true;
        }

        private void Current_Deactivated(object sender, EventArgs e)
        {
            applicationHasFocus = false;
            ResetMouseState();

            if (mouseInWindow)
            {
                mouseInWindow = false;
                if (HwndMouseLeave != null)
                    HwndMouseLeave(this, new HwndMouseEventArgs(mouseState));
            }

            ReleaseMouseCapture();
        }

        private void ResetMouseState()
        {
            // We need to invoke events for any buttons that were pressed
            bool fireL = mouseState.LeftButton == MouseButtonState.Pressed;
            bool fireM = mouseState.MiddleButton == MouseButtonState.Pressed;
            bool fireR = mouseState.RightButton == MouseButtonState.Pressed;
            bool fireX1 = mouseState.X1Button == MouseButtonState.Pressed;
            bool fireX2 = mouseState.X2Button == MouseButtonState.Pressed;

            // Update the state of all of the buttons
            mouseState.LeftButton = MouseButtonState.Released;
            mouseState.MiddleButton = MouseButtonState.Released;
            mouseState.RightButton = MouseButtonState.Released;
            mouseState.X1Button = MouseButtonState.Released;
            mouseState.X2Button = MouseButtonState.Released;

            // Fire any events
            HwndMouseEventArgs args = new HwndMouseEventArgs(mouseState);
            if (fireL && HwndLButtonUp != null)
                HwndLButtonUp(this, args);
            if (fireM && HwndMButtonUp != null)
                HwndMButtonUp(this, args);
            if (fireR && HwndRButtonUp != null)
                HwndRButtonUp(this, args);
            if (fireX1 && HwndX1ButtonUp != null)
                HwndX1ButtonUp(this, args);
            if (fireX2 && HwndX2ButtonUp != null)
                HwndX2ButtonUp(this, args);
            // The mouse is no longer considered to be in our window
            mouseInWindow = false;
        }


        private void GraphicsDeviceHost_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (graphicsService != null)
                graphicsService.ResetDevice((int)ActualWidth, (int)ActualHeight);
        }

        private void GraphicsDeviceHost_Loaded(object sender, RoutedEventArgs e)
        {
            // If we don't yet have a GraphicsDeviceService reference, we must add one for this control
            if (graphicsService == null)
            {
                graphicsService = GraphicsDeviceService.AddRef(hWnd, (int)ActualWidth, (int)ActualHeight);
            }
        }

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            // Create the host window as a child of the parent
            hWnd = CreateHostWindow(hwndParent.Handle);
            return new HandleRef(this, hWnd);
        }

        private IntPtr CreateHostWindow(IntPtr hWndParent)
        {
            // Register our window class
            RegisterWindowClass();

            // Create the window
            return NativeMethods.CreateWindowEx(0, windowClass, "",
               NativeMethods.WS_CHILD | NativeMethods.WS_VISIBLE,
               0, 0, (int)Width, (int)Height, hWndParent, IntPtr.Zero, IntPtr.Zero, 0);
        }

        private void RegisterWindowClass()
        {
            NativeMethods.WNDCLASSEX wndClass = new NativeMethods.WNDCLASSEX();
            wndClass.cbSize = (uint)Marshal.SizeOf(wndClass);
            wndClass.hInstance = NativeMethods.GetModuleHandle(null);
            wndClass.lpfnWndProc = NativeMethods.DefaultWindowProc;
            wndClass.lpszClassName = windowClass;
            wndClass.hCursor = NativeMethods.LoadCursor(IntPtr.Zero, NativeMethods.IDC_ARROW);

            NativeMethods.RegisterClassEx(ref wndClass);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            // Destroy the window and reset our hWnd value
            NativeMethods.DestroyWindow(hwnd.Handle);
            hWnd = IntPtr.Zero;
        }

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case NativeMethods.WM_MOUSEWHEEL:
                    if (mouseInWindow)
                    {
                        int delta = wParam.ToInt32();
                        if (HwndMouseWheel != null)
                            HwndMouseWheel(this, new HwndMouseEventArgs(mouseState, delta, 0));
                    }
                    break;
                case NativeMethods.WM_LBUTTONDOWN:
                    mouseState.LeftButton = MouseButtonState.Pressed;
                    if (HwndLButtonDown != null)
                        HwndLButtonDown(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_LBUTTONUP:
                    mouseState.LeftButton = MouseButtonState.Released;
                    if (HwndLButtonUp != null)
                        HwndLButtonUp(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_LBUTTONDBLCLK:
                    if (HwndLButtonDblClick != null)
                        HwndLButtonDblClick(this, new HwndMouseEventArgs(mouseState, MouseButton.Left));
                    break;
                case NativeMethods.WM_RBUTTONDOWN:
                    mouseState.RightButton = MouseButtonState.Pressed;
                    if (HwndRButtonDown != null)
                        HwndRButtonDown(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_RBUTTONUP:
                    mouseState.RightButton = MouseButtonState.Released;
                    if (HwndRButtonUp != null)
                        HwndRButtonUp(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_RBUTTONDBLCLK:
                    if (HwndRButtonDblClick != null)
                        HwndRButtonDblClick(this, new HwndMouseEventArgs(mouseState, MouseButton.Right));
                    break;
                case NativeMethods.WM_MBUTTONDOWN:
                    mouseState.MiddleButton = MouseButtonState.Pressed;
                    if (HwndMButtonDown != null)
                        HwndMButtonDown(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_MBUTTONUP:
                    mouseState.MiddleButton = MouseButtonState.Released;
                    if (HwndMButtonUp != null)
                        HwndMButtonUp(this, new HwndMouseEventArgs(mouseState));
                    break;
                case NativeMethods.WM_MBUTTONDBLCLK:
                    if (HwndMButtonDblClick != null)
                        HwndMButtonDblClick(this, new HwndMouseEventArgs(mouseState, MouseButton.Middle));
                    break;
                case NativeMethods.WM_XBUTTONDOWN:
                    if (((int)wParam & NativeMethods.MK_XBUTTON1) != 0)
                    {
                        mouseState.X1Button = MouseButtonState.Pressed;
                        if (HwndX1ButtonDown != null)
                            HwndX1ButtonDown(this, new HwndMouseEventArgs(mouseState));
                    }
                    else if (((int)wParam & NativeMethods.MK_XBUTTON2) != 0)
                    {
                        mouseState.X2Button = MouseButtonState.Pressed;
                        if (HwndX2ButtonDown != null)
                            HwndX2ButtonDown(this, new HwndMouseEventArgs(mouseState));
                    }
                    break;
                case NativeMethods.WM_XBUTTONUP:
                    if (((int)wParam & NativeMethods.MK_XBUTTON1) != 0)
                    {
                        mouseState.X1Button = MouseButtonState.Released;
                        if (HwndX1ButtonUp != null)
                            HwndX1ButtonUp(this, new HwndMouseEventArgs(mouseState));
                    }
                    else if (((int)wParam & NativeMethods.MK_XBUTTON2) != 0)
                    {
                        mouseState.X2Button = MouseButtonState.Released;
                        if (HwndX2ButtonUp != null)
                            HwndX2ButtonUp(this, new HwndMouseEventArgs(mouseState));
                    }
                    break;
                case NativeMethods.WM_XBUTTONDBLCLK:
                    if (((int)wParam & NativeMethods.MK_XBUTTON1) != 0)
                    {
                        if (HwndX1ButtonDblClick != null)
                            HwndX1ButtonDblClick(this, new HwndMouseEventArgs(mouseState, MouseButton.XButton1));
                    }
                    else if (((int)wParam & NativeMethods.MK_XBUTTON2) != 0)
                    {
                        if (HwndX2ButtonDblClick != null)
                            HwndX2ButtonDblClick(this, new HwndMouseEventArgs(mouseState, MouseButton.XButton2));
                    }
                    break;
                case NativeMethods.WM_MOUSEMOVE:
                    // If the application isn't in focus, we don't handle this message
                    if (!applicationHasFocus)
                        break;

                    // record the prevous and new position of the mouse
                    mouseState.PreviousPosition = mouseState.Position;
                    mouseState.Position = new Point(
                        NativeMethods.GetXLParam((int)lParam),
                        NativeMethods.GetYLParam((int)lParam));

                    if (!mouseInWindow)
                    {
                        mouseInWindow = true;

                        // if the mouse is just entering, use the same position for the previous state
                        // so we don't get weird deltas happening when the move event fires
                        mouseState.PreviousPosition = mouseState.Position;

                        if (HwndMouseEnter != null)
                            HwndMouseEnter(this, new HwndMouseEventArgs(mouseState));

                        hWndprev = NativeMethods.GetFocus();
                        NativeMethods.SetFocus(hWnd);
                        
                        // send the track mouse event so that we get the WM_MOUSELEAVE message
                        NativeMethods.TRACKMOUSEEVENT tme = new NativeMethods.TRACKMOUSEEVENT();
                        tme.cbSize = Marshal.SizeOf(typeof(NativeMethods.TRACKMOUSEEVENT));
                        tme.dwFlags = NativeMethods.TME_LEAVE;
                        tme.hWnd = hwnd;
                        NativeMethods.TrackMouseEvent(ref tme);
                    }

                    // Only fire the mouse move if the position actually changed
                    if (mouseState.Position != mouseState.PreviousPosition)
                    {
                        if (HwndMouseMove != null)
                            HwndMouseMove(this, new HwndMouseEventArgs(mouseState));
                    }

                    break;
                case NativeMethods.WM_MOUSELEAVE:

                    // If we have capture, we ignore this message because we're just
                    // going to reset the cursor position back into the window
                    if (isMouseCaptured)
                        break;

                    // Reset the state which releases all buttons and 
                    // marks the mouse as not being in the window.
                    ResetMouseState();

                    if (HwndMouseLeave != null)
                        HwndMouseLeave(this, new HwndMouseEventArgs(mouseState));

                    NativeMethods.SetFocus(hWndprev);
                    break;
                default:
                    break;
            }

            return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        }
    }
}
