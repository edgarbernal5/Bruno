using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace EsteroFramework.Graphics.Interop
{
    /// <summary>
    /// Represents the state of a mouse. Used by the GraphicsDeviceHost to track mouse state
    /// and for constructing the HwndMouseEventArgs.
    /// </summary>
    public class HwndMouseState
    {
        /// <summary>
        /// The current state of the left mouse button.
        /// </summary>
        public MouseButtonState LeftButton;

        /// <summary>
        /// The current state of the right mouse button.
        /// </summary>
        public MouseButtonState RightButton;

        /// <summary>
        /// The current state of the middle mouse button.
        /// </summary>
        public MouseButtonState MiddleButton;

        /// <summary>
        /// The current state of the first extra mouse button.
        /// </summary>
        public MouseButtonState X1Button;

        /// <summary>
        /// The current state of the second extra mouse button.
        /// </summary>
        public MouseButtonState X2Button;

        /// <summary>
        /// The current position of the mouse in screen coordinates.
        /// </summary>
        public Point ScreenPosition;
    }
}
