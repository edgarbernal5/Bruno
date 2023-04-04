
using System;
using System.Collections.Generic;

namespace Bruno.Collections
{
    public static class CollectionExtensions
    {
        internal static readonly WeakReference[] EmptyWeakReferenceArray = new WeakReference[0];


        public static void AddRange<T>(this ICollection<T> collection, IEnumerable<T> items)
        {
            if (collection == null)
                throw new ArgumentNullException("collection");
            if (items == null)
                throw new ArgumentNullException("items");

            var list = items as IList<T>;
            if (list != null)
            {
                int numberOfItems = list.Count;
                for (int i = 0; i < numberOfItems; i++)
                    collection.Add(list[i]);
            }
            else
            {
                foreach (T item in items)
                    collection.Add(item);
            }
        }
    }
}

