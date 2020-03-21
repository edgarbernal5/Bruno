using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Linq
{
    /// <summary>
    /// Provides new extension methods for LINQ.
    /// </summary>
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



        /// <summary>
        /// Returns an <see cref="IEnumerable{T}"/> that returns a single element.
        /// </summary>
        /// <typeparam name="T">The type of the element.</typeparam>
        /// <param name="value">The first and only element in the sequence.</param>
        /// <returns>
        /// An <see cref="IEnumerable{T}"/> that returns a single element.
        /// </returns>
        public static IEnumerable<T> Return<T>(T value)
        {
            yield return value;
        }
    }
}
