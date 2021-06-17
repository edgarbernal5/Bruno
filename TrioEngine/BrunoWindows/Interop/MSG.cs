using System;
using System.Runtime.InteropServices;

namespace BrunoWindows.Interop
{
    /// <summary>
    /// Contains message information from a thread's message queue. (See header file "winuser.h".)
    /// </summary>
    [CLSCompliant(false)]
    [StructLayout(LayoutKind.Sequential)]
    public struct MSG
    {
        /// <summary>
        /// Handle to the window whose window procedure receives the message.
        /// </summary>
        public IntPtr hWnd;

        /// <summary>
        /// Specifies the message identifier. Application can only use the low word; the high word
        /// is reserved by the system.
        /// </summary>
        public int message;

        /// <summary>
        /// Specifies additional information about the message. The exact meaning depends on the
        /// value of the message member.
        /// </summary>
        public IntPtr wParam;

        /// <summary>
        /// Specifies additional information about the message. The exact meaning depends on the
        /// value of the message member.
        /// </summary>
        public IntPtr lParam;

        /// <summary>
        /// Specifies the time at which the message was posted
        /// </summary>
        public int time;

        /// <summary>
        /// Specifies the cursor position, in screen coordinates, when the message was posted.
        /// </summary>
        public POINT pt;
    }

}
