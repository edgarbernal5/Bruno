using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using Estero;

namespace TrioWpfFramework.Windows
{
    /// <summary>
    /// Inverts a bool value.
    /// </summary>
    [ValueConversion(typeof(object), typeof(bool))]
    public class InvertBooleanConverter : IValueConverter
    {
        public static InvertBooleanConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new InvertBooleanConverter();

                return _instance;
            }
        }
        private static InvertBooleanConverter _instance;


        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                bool b = ObjectHelper.ConvertTo<bool>(value, culture);
                return !b;
            }
            catch
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
            return Convert(value, targetType, parameter, culture);
        }
    }
}
