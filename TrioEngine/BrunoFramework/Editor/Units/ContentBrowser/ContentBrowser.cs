
using BrunoWindows;
using System;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowser : PropertyChangedBase, IDisposable
    {
        public bool IsDisposed { get; private set; }

        //https://stackoverflow.com/questions/30750891/how-to-display-an-actual-image-in-listview-rather-than-its-location/30751033
        //https://www.codeproject.com/Questions/432800/Images-in-listview-WPF

        public ContentBrowserFileList FileListItems { get; } = new ContentBrowserFileList();
        public ObservableCollection<ContentBrowserItem> SelectedItems { get; } = new ObservableCollection<ContentBrowserItem>();

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!IsDisposed)
            {
                if (disposing)
                {

                }

                IsDisposed = true;
            }
        }

        public void Clear()
        {
            FileListItems.Clear();
        }
    }
}
