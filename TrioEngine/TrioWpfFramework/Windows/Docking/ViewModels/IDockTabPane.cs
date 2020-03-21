using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public interface IDockTabPane : IDockPane
    {
        IDockTabItem SelectedItem { get; set; }

        DockTabItemCollection Items { get; }
    }
}
