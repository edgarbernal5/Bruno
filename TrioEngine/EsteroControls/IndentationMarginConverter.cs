using Estero;
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace EsteroControls
{
    [ValueConversion(typeof(int), typeof(Thickness), ParameterType = typeof(double))]
    public class IndentationMarginConverter : IValueConverter
    {
        private static readonly object DefaultThickness = new Thickness();

        public static IndentationMarginConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new IndentationMarginConverter();

                return _instance;
            }
        }
        private static IndentationMarginConverter _instance;


        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            try
            {
                int level = ObjectHelper.ConvertTo<int>(value);
                if (level == 0)
                    return DefaultThickness;

                double size = ObjectHelper.ConvertTo<double>(parameter);
                return new Thickness(level * size, 0, 0, 0);
            }
            catch
            {
                return DependencyProperty.UnsetValue;
            }
        }


        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
