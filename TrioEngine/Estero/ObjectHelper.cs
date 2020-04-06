using System;

namespace Estero
{
    public static class ObjectHelper
    {
        public static T ConvertTo<T>(object value)
        {
            // TODO: For more advanced conversions take a look at Json.NET (Newtonsoft.Json\Src\Newtonsoft.Json\Utilities\ConvertUtils.cs).

            if (value is T)
                return (T)value;

            return (T)Convert.ChangeType(value, typeof(T));
        }

        public static T ConvertTo<T>(object value, IFormatProvider provider)
        {
            // TODO: For more advanced conversions take a look at Json.NET (Newtonsoft.Json\Src\Newtonsoft.Json\Utilities\ConvertUtils.cs).

            if (value is T)
                return (T)value;

            return (T)Convert.ChangeType(value, typeof(T), provider);
        }




        /// <summary>
        /// Safely disposes the object.
        /// </summary>
        /// <typeparam name="T">The type of the object.</typeparam>
        /// <param name="obj">The object to dispose. Can be <see langword="null"/>.</param>
        /// <remarks>
        /// The method calls <see cref="IDisposable.Dispose"/> if the <paramref name="obj"/> is not null
        /// and implements the interface <see cref="IDisposable"/>.
        /// </remarks>
        public static void SafeDispose<T>(this T obj) where T : class
        {
            var disposable = obj as IDisposable;
            if (disposable != null)
            {
                disposable.Dispose();
            }
        }
    }
}
