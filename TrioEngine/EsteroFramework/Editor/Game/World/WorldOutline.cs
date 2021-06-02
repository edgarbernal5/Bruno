
using EsteroWindows;
using System;
using System.Collections.ObjectModel;

namespace EsteroFramework.Editor.Game
{
    public class WorldOutline : PropertyChangedBase, IDisposable
    {
        public bool IsDisposed { get; private set; }

        public ObservableCollection<WorldOutlineItem> RootItems { get; } = new ObservableCollection<WorldOutlineItem>();

        public ObservableCollection<WorldOutlineItem> SelectedItems { get; } = new ObservableCollection<WorldOutlineItem>();

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
    }
}
