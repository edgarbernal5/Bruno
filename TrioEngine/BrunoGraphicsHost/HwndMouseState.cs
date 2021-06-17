
using System.Windows;
using System.Windows.Input;

namespace BrunoGraphicsHost
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
        /// The current position of the mouse.
        /// </summary>
        public Point Position;

        /// <summary>
        /// The previous position of the mouse.
        /// </summary>
        public Point PreviousPosition;
    }
}
