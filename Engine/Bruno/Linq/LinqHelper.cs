using System;
using System.Collections.Generic;

namespace Bruno.Linq
{
    public static class LinqHelper
    {
        public static void ForEach<T>(this IEnumerable<T> source, Action<T> action)
        {
            if (source == null)
                throw new ArgumentNullException("source");
            if (action == null)
                throw new ArgumentNullException("action");

            foreach (T element in source)
            {
                action(element);
            }
        }

        public static IEnumerable<T> Return<T>(T value)
        {
            yield return value;
        }
    }
}
