using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Allows the user to select a color from a <see cref="Controls.ColorPicker"/>.
    /// </summary>
    public partial class ColorDialog : Window
    {
        // Notes:
        // This dialog is not styleable. If you need a different style, just create your own
        // and use this code as a starting point.


        /// <summary>
        /// Gets or sets the old color.
        /// </summary>
        /// <value>The old color.</value>
        /// <remarks>
        /// This color will be displayed in the dialog to compare with the new color.
        /// </remarks>
        [TypeConverter(typeof(ColorConverter))]
        public Color OldColor
        {
            get { return ColorPicker.OldColor; }
            set { ColorPicker.OldColor = value; }
        }


        /// <summary>
        /// Gets or sets the color.
        /// </summary>
        /// <value>The color.</value>
        /// <remarks>
        /// The color selected with the <see cref="Controls.ColorPicker"/>.
        /// </remarks>
        [TypeConverter(typeof(ColorConverter))]
        public Color Color
        {
            get { return ColorPicker.Color; }
            set { ColorPicker.Color = value; }
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="ColorDialog"/> class.
        /// </summary>
        public ColorDialog()
        {
            InitializeComponent();
        }


        private void OnOkClick(object sender, RoutedEventArgs eventArgs)
        {
            eventArgs.Handled = true;
            DialogResult = true;
            Hide();
        }


        private void OnCancelClick(object sender, RoutedEventArgs eventArgs)
        {
            eventArgs.Handled = true;
            DialogResult = false;
            Hide();
            Color = OldColor;
        }
    }
}
