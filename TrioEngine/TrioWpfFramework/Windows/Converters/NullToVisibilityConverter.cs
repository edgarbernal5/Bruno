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
    [ValueConversion(typeof(object), typeof(Visibility))]

    public class NullToVisibilityConverter : IValueConverter
    {
        public static NullToVisibilityConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new NullToVisibilityConverter();

                return _instance;
            }
        }
        private static NullToVisibilityConverter _instance;

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return (value != null) ? Visibility.Visible : Visibility.Collapsed;
        }


        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
