using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Windows;

namespace TrioWpfFramework.Editor.WorldOutlines
{
    public class WorldOutline : ObservableObject
    {

        public ObservableCollection<WorldOutlineItem> RootItems { get; } = new ObservableCollection<WorldOutlineItem>();

        public ObservableCollection<WorldOutlineItem> SelectedItems { get; } = new ObservableCollection<WorldOutlineItem>();
    }
}
