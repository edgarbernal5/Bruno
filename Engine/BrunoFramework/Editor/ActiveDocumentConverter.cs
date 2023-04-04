
using System;
using System.Windows.Data;

namespace BrunoFramework.Editor
{
    public class ActiveDocumentConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			if (value is IEditorDockTab)
				return value;

			return Binding.DoNothing;
		}

		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			if (value is IEditorDockTab)
				return value;

			return Binding.DoNothing;
		}
	}
}
