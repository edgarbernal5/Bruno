
using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace BrunoFramework.Editor.Units
{
    [ValueConversion(typeof(Document), typeof(string))]
    public class DocumentToNameConverter : IValueConverter
    {
        public static DocumentToNameConverter Instance
        {
            get
            {
                if (_instance == null)
                    _instance = new DocumentToNameConverter();

                return _instance;
            }
        }
        private static DocumentToNameConverter _instance;

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var document = value as Document;
            if (document == null)
                return DependencyProperty.UnsetValue;

            return document.GetDisplayName();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
