using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Markup;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    [ContentProperty(nameof(ChildPane))]
    public class DockAnchorPaneViewModel : DockPaneViewModel, IDockAnchorPane
    {
        public IDockPane ChildPane
        {
            get { return _childPane; }
            set { SetProperty(ref _childPane, value); }
        }
        private IDockPane _childPane;
    }
}
