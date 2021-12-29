using System.Windows.Input;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Provides commands handled by the <see cref="ColorPicker"/>.
    /// </summary>
    public static class ColorPickerCommands
    {
        /// <summary>
        /// Gets the value that represents the <strong>Reset Color</strong> command.
        /// </summary>
        /// <value>The command.</value>
        public static RoutedCommand ResetColor
        {
            get
            {
                if (_resetColor == null)
                    _resetColor = new RoutedCommand("ResetColor", typeof(ColorPickerCommands));

                return _resetColor;
            }
        }
        private static RoutedCommand _resetColor;
    }
}
