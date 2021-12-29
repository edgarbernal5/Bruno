using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
{
    public class EditorPluginCollection : Collection<EditorPlugin>
    {
        internal bool IsLocked { get; set; }


        private void ThrowIfLocked()
        {
            if (IsLocked)
                throw new EditorException("Adding/removing extensions is not allowed after the editor was started.");
        }

        protected override void ClearItems()
        {
            ThrowIfLocked();
            base.ClearItems();
        }
        protected override void InsertItem(int index, EditorPlugin item)
        {
            ThrowIfLocked();
            if (item == null)
                throw new ArgumentNullException(nameof(item));
            if (Contains(item))
                throw new ArgumentException("Duplicate items are not allowed in the collection.", nameof(item));

            base.InsertItem(index, item);
        }

        protected override void RemoveItem(int index)
        {
            ThrowIfLocked();
            base.RemoveItem(index);
        }

        protected override void SetItem(int index, EditorPlugin item)
        {
            if (Items[index] == item)
                return;

            ThrowIfLocked();
            if (item == null)
                throw new ArgumentNullException(nameof(item));
            if (Contains(item))
                throw new ArgumentException("Duplicate items are not allowed in the collection.", nameof(item));

            base.SetItem(index, item);
        }
    }
}
