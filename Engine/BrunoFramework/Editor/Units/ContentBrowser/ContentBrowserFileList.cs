
using BrunoWindows;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserFileList : PropertyChangedBase
    {
        public ObservableCollection<ContentBrowserItem> Items { get; } = new ObservableCollection<ContentBrowserItem>();

        public void Clear()
        {
            Items.Clear();
        }
    }
}
