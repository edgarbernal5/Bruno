using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bruno.Collections
{
    public class NamedItemCollection<T> : KeyedCollection<string, T> where T : INamedItem
    {
        public NamedItemCollection()
            : base(StringComparer.Ordinal)
        {
        }

        public NamedItemCollection(IEqualityComparer<string> comparer)
            : base(comparer)
        {
        }

        public NamedItemCollection(IEqualityComparer<string> comparer, int dictionaryCreationThreshold)
            : base(comparer, dictionaryCreationThreshold)
        {
        }

        public new List<T>.Enumerator GetEnumerator()
        {
            return ((List<T>)Items).GetEnumerator();
        }
        protected override string GetKeyForItem(T item)
        {
            return item.Name;
        }

        public void AddRange(IEnumerable<T> collection)
        {
            if (collection == null)
                throw new ArgumentNullException("collection");

            foreach (var item in collection)
            {
                Add(item);
            }
        }

        public void InsertRange(int index, IEnumerable<T> collection)
        {
            if (collection == null)
                throw new ArgumentNullException("collection");

            foreach (var item in collection)
            {
                Insert(index, item);
                index++;
            }
        }

        public void RemoveRange(int index, int count)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException("index", "The starting index must be equal to or greater than 0.");
            if (count < 0)
                throw new ArgumentOutOfRangeException("count", "The number of elements to remove must be equal to or greater than 0.");
            if (index + count > Count)
                throw new ArgumentException("index and count do not denote a valid range of elements in the collection.");

            if (count == 0)
                return;


            for (int i = 0; i < count; i++)
            {
                RemoveAt(index);
            }
        }

        public bool TryGet(string key, out T value)
        {
            if (Dictionary == null)
            {
                // Linear search.
                foreach (var item in this)
                {
                    if (Comparer.Equals(item.Name, key))
                    {
                        value = item;
                        return true;
                    }
                }

                value = default(T);
                return false;
            }

            return Dictionary.TryGetValue(key, out value);
        }

        public bool TryGet<TExpected>(string key, out TExpected value) where TExpected : T
        {
            value = default(TExpected);

            if (Dictionary != null)
            {
                T t;
                if (Dictionary.TryGetValue(key, out t) && t is TExpected)
                {
                    value = (TExpected)t;
                    return true;
                }

                return false;
            }

            // No Dictionary --> Linear search.
            foreach (var item in this)
            {
                if (Comparer.Equals(item.Name, key))
                {
                    if (item is TExpected)
                    {
                        value = (TExpected)item;
                        return true;
                    }

                    return false;
                }
            }

            return false;
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