
using BrunoWindows;
using System;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutline : PropertyChangedBase, IDisposable
    {
        public ObservableCollection<WorldOutlineItem> RootItems { get; } = new ObservableCollection<WorldOutlineItem>();

        public ObservableCollection<WorldOutlineItem> SelectedItems { get; } = new ObservableCollection<WorldOutlineItem>();
    }
}
