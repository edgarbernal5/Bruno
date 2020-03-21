using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace TrioWpfFramework.Windows.Interop
{
    public static class Win32
    {
        public const uint TME_LEAVE = 0x00000002;
        public delegate IntPtr WndProc(IntPtr hWnd, uint msg, IntPtr wParam, IntPtr lParam);

        public static readonly WndProc DefaultWindowProc = DefWindowProc;

        /// <summary>
        /// Processes a default windows procedure.
        /// </summary>
        /// <param name="hWnd">Handle to the window procedure that received the message.</param>
        /// <param name="msg">The message.</param>
        /// <param name="wParam">
        /// Specifies additional message information. The content of this parameter depends on the
        /// value of the <paramref name="msg"/> parameter.
        /// </param>
        /// <param name="lparam">
        /// Specifies additional message information. The content of this parameter depends on the
        /// value of the <paramref name="msg"/> parameter.
        /// </param>
        /// <returns>
        /// The return value is the result of the message processing and depends on the message. If
        /// <paramref name="msg"/> is <see cref="WindowMessages.WM_SETTEXT"/>, zero is returned.
        /// </returns>
        [DllImport("user32.dll")]
        public static extern IntPtr DefWindowProc(IntPtr hWnd, uint uMsg, IntPtr wParam, IntPtr lParam);


        [DllImport("kernel32.dll")]
        public static extern IntPtr GetModuleHandle(string module);

        [DllImport("user32.dll", EntryPoint = "CreateWindowEx", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr CreateWindowEx(
            int exStyle,
            string className,
            string windowName,
            int style,
            int x, int y,
            int width, int height,
            IntPtr hwndParent,
            IntPtr hMenu,
            IntPtr hInstance,
            [MarshalAs(UnmanagedType.AsAny)] object pvParam);

        [DllImport("user32.dll", EntryPoint = "DestroyWindow", CharSet = CharSet.Auto)]
        public static extern bool DestroyWindow(IntPtr hwnd);

        /// <summary>
        /// Gets information about the specified window. The function also retrieves the 32-bit
        /// (long) value at the specified offset into the extra window memory.
        /// </summary>
        /// <param name="hWnd">
        /// Handle to the window and, indirectly, the class to which the window belongs.
        /// </param>
        /// <param name="nIndex">
        /// Specifies the zero-based offset to the value to be set. Valid values are in the range
        /// zero through the number of bytes of extra window memory, minus the size of an integer.
        /// To set any other value, specify one of the <see cref="GetWindowLongIndex"/> constants.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is the requested 32-bit value. If the
        /// function fails, the return value is zero. To get extended error information, call
        /// <see cref="Marshal.GetLastWin32Error"/>. If <see cref="SetWindowLong"/> has not been
        /// called previously, <see cref="GetWindowLong"/> returns zero for values in the extra
        /// window or class memory.
        /// </returns>
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern int GetWindowLong(IntPtr hWnd, int nIndex);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll")]
        public static extern int TrackMouseEvent(ref TRACKMOUSEEVENT lpEventTrack);

        [DllImport("user32.dll")]
        public static extern IntPtr LoadCursor(IntPtr hInstance, int lpCursorName);

        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.U2)]
        public static extern short RegisterClassEx([In] ref WNDCLASSEX lpwcx);

        [DllImport("user32.dll")]
        public static extern int ShowCursor(bool bShow);

        /// <summary>
        /// Gets the mouse cursor's position, in screen coordinates.
        /// </summary>
        /// <param name="lpPoint">The screen coordinates of the cursor.</param>
        /// <returns>
        /// If the function succeeds, the return value is <see langword="true"/>. If the function
        /// fails, the return value is <see langword="false"/>. To get extended error information,
        /// call <see cref="Marshal.GetLastWin32Error"/>.
        /// </returns>
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetCursorPos(ref POINT lpPoint);

        [DllImport("user32.dll")]
        public static extern bool SetCursorPos(int x, int y);

        /// <summary>
        /// Converts the screen coordinates of a specified point on the screen to client-area
        /// coordinates.
        /// </summary>
        /// <param name="hWnd">
        /// Handle to the window whose client area will be used for the conversion.
        /// </param>
        /// <param name="lpPoint">the screen coordinates to be converted.</param>
        /// <returns>
        /// If the function succeeds, the return value is <see langword="true"/>. If the function
        /// fails, the return value is <see langword="false"/>. To get extended error information,
        /// call <see cref="Marshal.GetLastWin32Error"/>.
        /// </returns>
        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ScreenToClient(IntPtr hWnd, ref POINT lpPoint);

        /// <summary>
        /// Gets the handle to the window that has the keyboard focus, if the window is attached to
        /// the calling thread's message queue.
        /// </summary>
        /// <returns>
        /// The handle to the window with the keyboard focus. If the calling thread's message queue
        /// does not have an associated window with the keyboard focus, the return value is
        /// <see langword="null"/>.
        /// </returns>
        [DllImport("user32.dll")]
        public static extern IntPtr GetFocus();

        /// <summary>
        /// Sets the keyboard focus to the specified window. The window must be attached to the
        /// calling thread's message queue.
        /// </summary>
        /// <param name="hWnd">
        /// Handle to the window that will receive the keyboard input. If this parameter is
        /// <see langword="null"/>, keystrokes are ignored.
        /// </param>
        /// <returns>
        /// If the function succeeds, the return value is the handle to the window that previously
        /// had the keyboard focus. If the hWnd parameter is invalid or the window is not attached
        /// to the calling thread's message queue, the return value is <see langword="null"/>.
        /// </returns>
        [DllImport("user32.dll")]
        public static extern IntPtr SetFocus(IntPtr hWnd);

        /// <summary>
        /// Changes the size, position, and Z order of a child, pop-up, or top-level window. These
        /// windows are ordered according to their appearance on the screen. The topmost window
        /// receives the highest rank and is the first window in the Z order.
        /// </summary>
        /// <param name="hWnd">A handle to the window.</param>
        /// <param name="hWndAfter">
        /// A handle to the window to precede the positioned window in the Z order. This parameter
        /// must be a window handle or one of the following values:
        /// <see cref="HwndConstants.HWND_BOTTOM"/>, <see cref="HwndConstants.HWND_NOTOPMOST"/>,
        /// <see cref="HwndConstants.HWND_TOP"/>, or <see cref="HwndConstants.HWND_TOPMOST"/>.
        /// </param>
        /// <param name="X">
        /// The new position of the left side of the window, in client coordinates.
        /// </param>
        /// <param name="Y">
        /// The new position of the top of the window, in client coordinates.
        /// </param>
        /// <param name="cx">The new width of the window, in pixels.</param>
        /// <param name="cy">The new height of the window, in pixels.</param>
        /// <param name="flags">The window sizing and positioning flags.</param>
        /// <returns>
        /// If the function succeeds, the return value is <see langword="true"/>. If the function
        /// fails, the return value is <see langword="false"/>. To get extended error information,
        /// call <see cref="Marshal.GetLastWin32Error"/>.
        /// </returns>
        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndAfter, int X, int Y, int cx, int cy, SetWindowPosFlags flags);

        /// <summary>
        /// Releases the mouse capture from a window in the current thread and restores normal mouse
        /// input processing.
        /// </summary>
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ReleaseCapture();


        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ShowWindow(IntPtr hWnd, ShowWindowStyles nCmdShow);

        /// <summary>
        /// Returns a DWORD value by concatenation the specified values.
        /// </summary>
        /// <param name="low">The low-order word of the new value.</param>
        /// <param name="high">The high-order word of the new value.</param>
        /// <returns>An unsigned 32-bit value (DWORD).</returns>
        public static uint MAKELONG(int low, int high)
        {
            return (uint)((high << 16) + low);
        }


        /// <summary>
        /// Gets the signed x-coordinate from the given lParam value.
        /// </summary>
        /// <param name="lParam">The value to be converted.</param>
        /// <returns>The signed x-coordinate.</returns>
        public static int GET_X_LPARAM(IntPtr lParam)
        {
            return LOWORD(lParam.ToInt32());
        }


        /// <summary>
        /// Gets the signed y-coordinate from the given lParam value.
        /// </summary>
        /// <param name="lParam">The value to be converted.</param>
        /// <returns>The signed y-coordinate.</returns>
        public static int GET_Y_LPARAM(IntPtr lParam)
        {
            return HIWORD(lParam.ToInt32());
        }


        /// <summary>
        /// Gets the low-order word from the specified value.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The low-order word.</returns>
        public static int LOWORD(int value)
        {
            return value & 0xFFFF;
        }


        /// <summary>
        /// Gets the high-order word from the specified value.
        /// </summary>
        /// <param name="value">The value to be converted.</param>
        /// <returns>The high-order word.</returns>
        public static int HIWORD(int value)
        {
            return value >> 16;
        }
    }
}
