using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Converts an <see cref="KeyGesture"/> to a string that represents the key gesture that
    /// executes the command item.
    /// </summary>
    [ValueConversion(typeof(KeyGesture), typeof(string))]
    public class KeyGestureToStringConverter : IValueConverter
    {
        // Note: 
        // I first used our ValueToStringConverter which uses the original 
        // System.Windows.Input.KeyGestureConverter. But for a "Ctrl+X" key gesture this 
        // converter.ConvertToString(...) returned "Ctrl+X, Ctrl+X".
        // --> I don't know what is going wrong there, so we use our own converter.


        // The System.Windows.Input.KeyGestureConverter.
        private static readonly KeyGestureConverter KeyGestureConverter = new KeyGestureConverter();


        /// <summary>
        /// An instance of the <see cref="KeyGestureToStringConverter"/>.
        /// </summary>
        public static readonly KeyGestureToStringConverter Instance = new KeyGestureToStringConverter();


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
            if (value == null)
                return null;

            var gesture = value as KeyGesture;
            if (gesture == null)
                return DependencyProperty.UnsetValue;

            if (!string.IsNullOrEmpty(gesture.DisplayString))
                return gesture.DisplayString;

            return KeyGestureConverter.ConvertTo(null, culture, gesture, typeof(string));
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
