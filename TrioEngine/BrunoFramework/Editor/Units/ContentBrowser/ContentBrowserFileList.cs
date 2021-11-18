using BrunoWindows;
using System;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserFileList : PropertyChangedBase
    {
        public ObservableCollection<ContentBrowserFileListItem> Items { get; } = new ObservableCollection<ContentBrowserFileListItem>();

        public void Clear()
        {
            Items.Clear();
        }
    }
}
