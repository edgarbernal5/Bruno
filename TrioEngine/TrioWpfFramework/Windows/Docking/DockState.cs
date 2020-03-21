using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking
{
    /// <summary>
    /// Defines possible states in the docking layout.
    /// </summary>
    public enum DockState
    {
        /// <summary>
        /// The element is hidden.
        /// </summary>
        Hide,

        /// <summary>
        /// The element is docked in the <see cref="IDockControl"/>.
        /// </summary>
        Dock,

        /// <summary>
        /// The element is docked in one of the <see cref="IDockControl.FloatWindows"/>.
        /// </summary>
        Float,

        /// <summary>
        /// The element is in the left, right, top, or bottom auto-hide bar.
        /// </summary>
        AutoHide,
    }
}
