
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace TrioWpfFramework.Windows
{
    [ValueConversion(typeof(Enum), typeof(Array))]
    public class EnumToArrayConverter : IValueConverter
    {
        /// <summary>
        /// Gets an instance of the <see cref="EnumToArrayConverter"/>.
        /// </summary>
        /// <value>An instance of the <see cref="EnumToArrayConverter"/>.</value>
        public static EnumToArrayConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new EnumToArrayConverter();

                return _instance;
            }
        }
        private static EnumToArrayConverter _instance;


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
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                return Enum.GetValues(value.GetType());
            }
            catch
            {
                return DependencyProperty.UnsetValue;
            }
        }


        /// <summary>
        /// Not implemented.
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
