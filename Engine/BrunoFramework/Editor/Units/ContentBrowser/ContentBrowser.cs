
using BrunoWindows;
using System;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowser : PropertyChangedBase, IDisposable
    {
        public delegate void ContentFileEvent(ContentBrowserItem item);
        public delegate void ContentFolderEvent(string path);
        public delegate void ContentFilterByTypeEvent(FilterByType filterByType);
        public event ContentFileEvent DoubleClickFileItem;
        public event ContentFolderEvent DoubleClickFolderItem;
        public event ContentFilterByTypeEvent ChangedFilterByType;

        public string CurrentPath
        {
            get => m_currentPath;
            set
            {
                m_currentPath = value;
                //NotifyOfPropertyChange();
            }
        }
        private string m_currentPath = "";

        public FilterByType FilterByType
        {
            get => m_filterByType;
            set
            {
                m_filterByType = value;
            }
        }
        private FilterByType m_filterByType = FilterByType.All;

        //https://stackoverflow.com/questions/30750891/how-to-display-an-actual-image-in-listview-rather-than-its-location/30751033
        //https://www.codeproject.com/Questions/432800/Images-in-listview-WPF
        //controls:ListViewExtentions.SelectedItems="{Binding Content.SelectedItems}"

        public ContentBrowserFileTree FileTreeItems { get; } = new ContentBrowserFileTree();

        public ContentBrowserFileList FileListItems { get; } = new ContentBrowserFileList();

        public void ClearAll()
        {
            ClearFileList();
            ClearFileTree();
        }

        public void ClearFileList()
        {
            FileListItems.Clear();
        }

        public void ClearFileTree()
        {
            FileTreeItems.Clear();
        }

        public void OnDoubleClickFileItem(ContentBrowserItem item)
        {
            DoubleClickFileItem?.Invoke(item);
        }

        public void OnDoubleClickFolderItem(string path)
        {
            CurrentPath = path;
            DoubleClickFolderItem?.Invoke(path);
        }

        public void OnFilterByType(FilterByType filterByType)
        {
            FilterByType = filterByType;
            ChangedFilterByType?.Invoke(filterByType);
        }
    }
}
