using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Markup;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{

    /// <summary>
    /// Represents a pane in the docking layout that is split horizontally or vertically into two
    /// or more panes.
    /// </summary>
    [ContentProperty(nameof(ChildPanes))]
    public class DockSplitPaneViewModel : DockPaneViewModel, IDockSplitPane
    {
        public Orientation Orientation
        {
            get { return _orientation; }
            set { SetProperty(ref _orientation, value); }
        }
        private Orientation _orientation;

        public DockPaneCollection ChildPanes { get; } = new DockPaneCollection();
    }
}
