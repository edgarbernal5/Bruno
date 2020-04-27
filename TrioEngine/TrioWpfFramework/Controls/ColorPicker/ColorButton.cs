﻿using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using TrioWpfFramework.Windows;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Represents a control that displays a color and opens a <see cref="ColorDialog"/> when
    /// clicked.
    /// </summary>
    [TemplatePart(Name = "PART_Button", Type = typeof(ButtonBase))]
    public class ColorButton : Control
    {
        // Notes:
        // The color dialog is not styleable. If you need a different style, just create your own
        // Color button and color dialog and use this code as a starting point.


        private ButtonBase _button;

        /// <summary>
        /// Identifies the <see cref="Color"/> dependency property.
        /// </summary>
        public static readonly DependencyProperty ColorProperty = DependencyProperty.Register(
            "Color",
            typeof(Color),
            typeof(ColorButton),
            new FrameworkPropertyMetadata(Boxed.ColorBlack, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

        /// <summary>
        /// Gets or sets the color. 
        /// This is a dependency property.
        /// </summary>
        /// <value>The <see cref="System.Windows.Media.Color"/> value.</value>
        [Description("Gets or sets the color.")]
        [Category(Categories.Default)]
        [TypeConverter(typeof(ColorConverter))]
        public Color Color
        {
            get { return (Color)GetValue(ColorProperty); }
            set { SetValue(ColorProperty, value); }
        }


        /// <summary>
        /// Initializes static members of the <see cref="ColorButton"/> class.
        /// </summary>
        static ColorButton()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(ColorButton), new FrameworkPropertyMetadata(typeof(ColorButton)));
        }

        /// <summary>
        /// When overridden in a derived class, is invoked whenever application code or internal
        /// processes call <see cref="FrameworkElement.ApplyTemplate"/>.
        /// </summary>
        public override void OnApplyTemplate()
        {
            // Clean up.
            if (_button != null)
            {
                _button.Click -= OnButtonClick;
                _button = null;
            }

            base.OnApplyTemplate();

            // Find new button and install event handler.
            _button = GetTemplateChild("PART_Button") as ButtonBase;
            if (_button != null)
            {
                _button.Click += OnButtonClick;
            }
        }


        private void OnButtonClick(object sender, RoutedEventArgs eventArgs)
        {
            // Start color picker dialog and use new color.
            var dialog = new ColorDialog
            {
                OldColor = Color,
                Color = Color
            };

            dialog.ShowDialog();

            if (dialog.DialogResult.GetValueOrDefault())
                Color = dialog.Color;
        }
    }
}
