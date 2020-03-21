using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking
{
    /// <summary>
    /// Defines a position relative to a target pane in a docking layout.
    /// </summary>
    public enum DockPosition
    {
        /// <summary>
        /// Undefined position.
        /// </summary>
        None,

        /// <summary>
        /// The element is docked left of the target pane.
        /// </summary>
        Left,

        /// <summary>
        /// The element is docked right of the target pane.
        /// </summary>
        Right,

        /// <summary>
        /// The element is docked above the target pane.
        /// </summary>
        Top,

        /// <summary>
        /// The element is docked below the target pane.
        /// </summary>
        Bottom,

        /// <summary>
        /// The element is docked inside the target pane.
        /// </summary>
        Inside
    }
}
