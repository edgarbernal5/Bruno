using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Delivery.ViewModels.Core;

namespace TrioWpfEditor.Delivery.ViewModels.WorldTrees
{
    public class WorldTree : ObservableObject
    {

        public ObservableCollection<WorldTreeItem> RootItems { get; } = new ObservableCollection<WorldTreeItem>();

        public ObservableCollection<WorldTreeItem> SelectedItems { get; } = new ObservableCollection<WorldTreeItem>();
    }
}
