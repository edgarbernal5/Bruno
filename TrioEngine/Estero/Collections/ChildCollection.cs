﻿using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace Estero.Collections
{
    public abstract class ChildCollection<TParent, TChild> : Collection<TChild>
        where TParent : class
        where TChild : class
    {
        public TParent Parent
        {
            get { return _parent; }
            protected set
            {
                if (_parent == value)
                    return;

                _parent = value;

                foreach (TChild child in (List<TChild>)Items)
                {
                    SetParent(child, _parent);
                }
            }
        }
        private TParent _parent;

        protected abstract TParent GetParent(TChild child);

        protected abstract void SetParent(TChild child, TParent parent);

        protected ChildCollection(TParent parent)
        {
            _parent = parent;
        }

        protected ChildCollection(TParent parent, int capacity) 
            : base(new List<TChild>(capacity))
        {
            _parent = parent;
        }

        public new List<TChild>.Enumerator GetEnumerator()
        {
            return ((List<TChild>)Items).GetEnumerator();
        }
        protected override void ClearItems()
        {
            foreach (TChild child in (List<TChild>)Items)
            {
                SetParent(child, null);
            }

            base.ClearItems();
        }

        protected override void InsertItem(int index, TChild item)
        {
            TParent parent = GetParent(item);
            if (parent != null)
            {
                if (parent == _parent)
                    throw new InvalidOperationException("Cannot insert item into children collection. Item is already part of this collection.");
                else
                    throw new InvalidOperationException("Cannot insert item into children collection. Item is already the child of another object.");
            }

            base.InsertItem(index, item);
            SetParent(item, _parent);
        }

        protected override void RemoveItem(int index)
        {
            TChild removedChild = Items[index];
            base.RemoveItem(index);
            SetParent(removedChild, null);
        }

        protected override void SetItem(int index, TChild item)
        {
            if (item == null)
                throw new ArgumentNullException("item", "Items in a children collection must not be null.");

            TChild removedItem = Items[index];
            if (!ReferenceEquals(item, removedItem))
            {
                TParent parent = GetParent(item);
                if (parent != null)
                {
                    if (parent == _parent)
                        throw new InvalidOperationException("Cannot insert item into children collection. Item is already part of this collection.");
                    else
                        throw new InvalidOperationException("Cannot insert item into children collection. Item is already the child of another object.");
                }

                base.SetItem(index, item);
                SetParent(removedItem, null);
                SetParent(item, _parent);
            }
        }

        public void Move(int oldIndex, int newIndex)
        {
            if (oldIndex < 0 || oldIndex >= Count)
                throw new ArgumentOutOfRangeException("oldIndex");
            if (newIndex < 0 || newIndex >= Count)
                throw new ArgumentOutOfRangeException("oldIndex");

            if (oldIndex == newIndex)
                return;

            MoveItem(oldIndex, newIndex);
        }

        protected virtual void MoveItem(int oldIndex, int newIndex)
        {
            var item = Items[oldIndex];
            Items.RemoveAt(oldIndex);
            Items.Insert(newIndex, item);
        }
    }
}
