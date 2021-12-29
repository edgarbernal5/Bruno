using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;

namespace TrioWpfFramework.Windows
{
    /// <summary>
    /// Converts a string to uppercase.
    /// </summary>
    [ValueConversion(typeof(string), typeof(string))]
    public class ToUpperConverter : IValueConverter
    {
        public static ToUpperConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new ToUpperConverter();

                return _instance;
            }
        }
        private static ToUpperConverter _instance;


        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null)
                return DependencyProperty.UnsetValue;

            string valueAsString = value as string;

            // If the given value is not a string, then we convert the value.ToString() result.
            if (valueAsString == null)
                valueAsString = value.ToString();

            return valueAsString.ToUpper(culture);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
