using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public interface IFloatWindow : IDockContainer
    {
        /// <summary>
        /// Gets (or sets) a value indicating whether this window is visible.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if this window is visible; otherwise, <see langword="false"/>.
        /// </value>
        /// <remarks>
        /// This property is set automatically by the <see cref="DockStrategy"/>.
        /// </remarks>
        bool IsVisible { get; set; }


        /// <summary>
        /// Gets or sets the position for the window's left edge.
        /// </summary>
        /// <value>The left border position of a window.</value>
        double Left { get; set; }


        /// <summary>
        /// Gets or sets the position of the window's top edge.
        /// </summary>
        /// <value>The top border position of the window.</value>
        double Top { get; set; }


        /// <summary>
        /// Gets or sets the width of the window.
        /// </summary>
        /// <value>The width of the window.</value>
        double Width { get; set; }


        /// <summary>
        /// Gets or sets the height of the window.
        /// </summary>
        /// <value>The height of the window.</value>
        double Height { get; set; }


        /// <summary>
        /// Gets or sets the state (normal, minimized, or maximized) of the window.
        /// </summary>
        /// <value>The state (normal, minimized, or maximized) of the window.</value>
        WindowState WindowState { get; set; }
    }
}
