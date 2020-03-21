using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public abstract class DockElementCollection<T> : ObservableCollection<T>
    {
        public new List<T>.Enumerator GetEnumerator()
        {
            return ((List<T>)Items).GetEnumerator();
        }

        protected override void InsertItem(int index, T item)
        {
            if (item == null)
                throw new ArgumentNullException(nameof(item));
            if (Contains(item))
                throw new ArgumentException("Duplicate items are not allowed in the collection.", nameof(item));

            base.InsertItem(index, item);
        }

        protected override void SetItem(int index, T item)
        {
            T removedObject = Items[index];
            if (EqualityComparer<T>.Default.Equals(item, removedObject))
                return;

            if (item == null)
                throw new ArgumentNullException(nameof(item));
            if (Contains(item))
                throw new ArgumentException("Duplicate items are not allowed in the collection.");

            base.SetItem(index, item);
        }
    }
}
