using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking
{
    public enum AutoHideState
    {
        /// <summary>
        /// The <see cref="AutoHidePane"/> is hidden. (Slid out of the visible area.)
        /// </summary>
        Hidden,

        /// <summary>
        /// The <see cref="AutoHidePane"/> is currently animated and moved in to the visible area.
        /// (It is partially visible.)
        /// </summary>
        SlidingIn,

        /// <summary>
        /// The <see cref="AutoHidePane"/> is fully visible.
        /// </summary>
        Shown,

        /// <summary>
        /// The <see cref="AutoHidePane"/> is currently animated and moved out of the visible area.
        /// (It is partially visible.)
        /// </summary>
        SlidingOut
    }
}
