using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using TrioWpfFramework.Controls;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Converts a bitmap source to an <see cref="Icon"/> for use in a menu item.
    /// </summary>
    [ValueConversion(typeof(CommandItem), typeof(string))]
    public class MenuIconConverter : IValueConverter
    {
        /// <summary>
        /// An instance of the <see cref="MenuIconConverter"/>.
        /// </summary>
        public static readonly MenuIconConverter Instance = new MenuIconConverter();


        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value produced by the binding source.</param>
        /// <param name="targetType">The type of the binding target property.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns <see langword="null"/>, the valid null value is 
        /// used.
        /// </returns>
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                if (value == null)
                    return null;

                var image = new Icon
                {
                    Source = value,
                    Width = 16,
                    Height = 16,
                };
                return image;
            }
            catch (Exception)
            {
                return DependencyProperty.UnsetValue;
            }
        }


        /// <summary>
        /// Converts a value.
        /// </summary>
        /// <param name="value">The value that is produced by the binding target.</param>
        /// <param name="targetType">The type to convert to.</param>
        /// <param name="parameter">The converter parameter to use.</param>
        /// <param name="culture">The culture to use in the converter.</param>
        /// <returns>
        /// A converted value. If the method returns <see langword="null"/>, the valid null value is 
        /// used.
        /// </returns>
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
