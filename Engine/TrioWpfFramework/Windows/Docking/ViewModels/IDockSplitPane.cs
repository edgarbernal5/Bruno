using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace TrioWpfFramework.Windows.Docking
{
    public interface IDockSplitPane : IDockPane
    {
        /// <summary>
        /// Gets or sets the orientation.
        /// </summary>
        /// <value>
        /// The orientation. <see cref="System.Windows.Controls.Orientation.Horizontal"/> means that
        /// the child panes are arranged horizontally next to each other.
        /// <see cref="System.Windows.Controls.Orientation.Vertical"/> means that the child panes
        /// are stacked vertically.
        /// </value>
        Orientation Orientation { get; set; }


        /// <summary>
        /// Gets the child panes.
        /// </summary>
        /// <value>The child panes. Must not be <see langword="null"/>.</value>
        DockPaneCollection ChildPanes { get; }
    }
}
