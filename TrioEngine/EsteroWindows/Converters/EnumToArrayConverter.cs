using System;
using System.Globalization;
using System.Windows.Data;

namespace EsteroWindows
{
    public class EnumToArrayConverter : IValueConverter
    {
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

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return Enum.GetValues(value.GetType());
        }
        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
