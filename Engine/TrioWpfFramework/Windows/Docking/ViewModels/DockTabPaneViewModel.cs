using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Markup;

namespace TrioWpfFramework.Windows.Docking
{
    [ContentProperty(nameof(Items))]
    public class DockTabPaneViewModel : DockPaneViewModel, IDockTabPane
    {
        public IDockTabItem SelectedItem
        {
            get { return _selectedItem; }
            set { SetProperty(ref _selectedItem, value); }
        }
        private IDockTabItem _selectedItem;

        public DockTabItemCollection Items { get; } = new DockTabItemCollection();
    }
}
