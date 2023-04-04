
using BrunoWindows;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserFileTree : PropertyChangedBase
    {
        public ObservableCollection<ContentBrowserTreeItem> Items { get; } = new ObservableCollection<ContentBrowserTreeItem>();
        public BindableCollection<ContentBrowserTreeItem> SelectedItems { get; } = new BindableCollection<ContentBrowserTreeItem>();
        
        public void Clear()
        {
            Items.Clear();
        }
    }
}
