
using BrunoWindows;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutline : PropertyChangedBase
    {
        private readonly IWorldOutlineHandler m_worldOutlineHandler;

        public ObservableCollection<WorldOutlineItem> RootItems { get; } = new ObservableCollection<WorldOutlineItem>();

        public BindableCollection<WorldOutlineItem> SelectedItems { get; } = new BindableCollection<WorldOutlineItem>();

        public WorldOutline(IWorldOutlineHandler worldOutlineHandler)
        {
            m_worldOutlineHandler = worldOutlineHandler;

            SelectedItems.CollectionChanged += OnSelectedItemsCollectionChanged;
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                SelectedItems.CollectionChanged -= OnSelectedItemsCollectionChanged;
            }
            base.OnDisposing(disposing);
        }

        private void OnSelectedItemsCollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            m_worldOutlineHandler.SelectionChanged();
        }

        public void RemoveItem(WorldOutlineItem item)
        {
            if (item.Parent == null)
            {
                RootItems.Remove(item);
            }
            else
            {
                item.Parent.Children.Remove(item);
            }
        }

        public void OnDeleteObjects(List<long> entities)
        {
            m_worldOutlineHandler.DeleteObjects(entities);
        }

        public void AddEmptyObject(string name)
        {
            m_worldOutlineHandler.AddEmptyObject(name);
        }
    }
}
