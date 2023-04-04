
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;

namespace BrunoWindows
{
    public class BindableCollection<T> : ObservableCollection<T>
    {
        private bool m_suppressNotifications;

        public bool IsNotifying
        {
            get { return !m_suppressNotifications; }
            set { m_suppressNotifications = !value; }
        }

        public BindableCollection()
        {
        }

        public BindableCollection(IEnumerable<T> collection)
        {
            AddRange(collection);
        }
        public virtual bool ShouldSerializeIsNotifying()
        {
            return false;
        }

        public void Refresh()
        {
            WindowsPlatform.OnUIThread(() =>
            {
                OnPropertyChanged(new PropertyChangedEventArgs("Count"));
                OnPropertyChanged(new PropertyChangedEventArgs("Item[]"));
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            });
        }


        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs e)
        {
            if (!m_suppressNotifications)
                base.OnCollectionChanged(e);
        }

        protected override void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            if (!m_suppressNotifications)
                base.OnPropertyChanged(e);
        }

        public void AddRange(IEnumerable<T> items)
        {
            WindowsPlatform.OnUIThread(() =>
            {
                bool previousSuppressNotifications = m_suppressNotifications;
                m_suppressNotifications = true;
                try
                {
                    var index = Count;
                    foreach (var item in items)
                    {
                        InsertItemBase(index, item);
                        index++;
                    }
                }
                finally
                {
                    m_suppressNotifications = previousSuppressNotifications;
                }

                OnPropertyChanged(new PropertyChangedEventArgs("Count"));
                OnPropertyChanged(new PropertyChangedEventArgs("Item[]"));
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            });
        }

        public void RemoveRange(IEnumerable<T> items)
        {
            WindowsPlatform.OnUIThread(() =>
            {
                bool previousSuppressNotifications = m_suppressNotifications;
                m_suppressNotifications = true;
                try
                {
                    foreach (var item in items)
                    {
                        var index = IndexOf(item);
                        if (index >= 0)
                            RemoveItemBase(index);
                    }
                }
                finally
                {
                    m_suppressNotifications = previousSuppressNotifications;
                }

                OnPropertyChanged(new PropertyChangedEventArgs("Count"));
                OnPropertyChanged(new PropertyChangedEventArgs("Item[]"));
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            });
        }

        protected sealed override void ClearItems()
        {
            WindowsPlatform.OnUIThread(ClearItemsBase);
        }

        protected virtual void ClearItemsBase()
        {
            base.ClearItems();
        }

        protected sealed override void InsertItem(int index, T item)
        {
            WindowsPlatform.OnUIThread(() => InsertItemBase(index, item));
        }

        protected virtual void InsertItemBase(int index, T item)
        {
            base.InsertItem(index, item);
        }

        protected sealed override void MoveItem(int oldIndex, int newIndex)
        {
            WindowsPlatform.OnUIThread(() => MoveItemBase(oldIndex, newIndex));
        }

        protected virtual void MoveItemBase(int oldIndex, int newIndex)
        {
            base.MoveItem(oldIndex, newIndex);
        }

        protected sealed override void RemoveItem(int index)
        {
            WindowsPlatform.OnUIThread(() => RemoveItemBase(index));
        }

        protected virtual void RemoveItemBase(int index)
        {
            base.RemoveItem(index);
        }

        protected sealed override void SetItem(int index, T item)
        {
            WindowsPlatform.OnUIThread(() => SetItemBase(index, item));
        }
        protected virtual void SetItemBase(int index, T item)
        {
            base.SetItem(index, item);
        }

    }
}
