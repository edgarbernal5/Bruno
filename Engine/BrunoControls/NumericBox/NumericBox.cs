
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace BrunoControls
{
    public class NumericBox : Control
    {
        private TextBlock _textBlock;
        private TextBox _textBox;

        static NumericBox()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(NumericBox),
                new FrameworkPropertyMetadata(typeof(NumericBox)));
        }

        public static readonly DependencyProperty ValueProperty = DependencyProperty.Register(
            "Value", typeof(double), typeof(NumericBox),
            new FrameworkPropertyMetadata(0.0, FrameworkPropertyMetadataOptions.BindsTwoWayByDefault, null, OnCoerceValue));

        private static object OnCoerceValue(DependencyObject d, object basevalue)
        {
            return ((NumericBox)d).CoerceValue((double)basevalue);
        }

        public double Value
        {
            get { return (double)GetValue(ValueProperty); }
            set { SetValue(ValueProperty, value); }
        }

        public static readonly DependencyProperty MinimumProperty = DependencyProperty.Register(
            "Minimum", typeof(double?), typeof(NumericBox));

        public double? Minimum
        {
            get { return (double?)GetValue(MinimumProperty); }
            set { SetValue(MinimumProperty, value); }
        }

        public static readonly DependencyProperty MaximumProperty = DependencyProperty.Register(
            "Maximum", typeof(double?), typeof(NumericBox));

        public double? Maximum
        {
            get { return (double?)GetValue(MaximumProperty); }
            set { SetValue(MaximumProperty, value); }
        }

        public static readonly DependencyProperty ModeProperty = DependencyProperty.Register(
            "Mode", typeof(NumericBoxMode), typeof(NumericBox),
            new FrameworkPropertyMetadata(NumericBoxMode.Normal));

        public NumericBoxMode Mode
        {
            get { return (NumericBoxMode)GetValue(ModeProperty); }
            set { SetValue(ModeProperty, value); }
        }

        public override void OnApplyTemplate()
        {
            _textBlock = (TextBlock)Template.FindName("TextBlock", this);

            var mouseMoved = false;
            var previousPosition = new Point();

            _textBlock.MouseDown += (sender, eventArgs) =>
            {
                previousPosition = eventArgs.GetPosition(_textBlock);
                _textBlock.CaptureMouse();
                mouseMoved = false;
            };

            _textBlock.MouseMove += (sender, eventArgs) =>
            {
                if (!_textBlock.IsMouseCaptured)
                    return;

                mouseMoved = true;

                var newPosition = eventArgs.GetPosition(_textBlock);
                Value = CoerceValue(Value + (newPosition.X - previousPosition.X) / 50.0);

                previousPosition = newPosition;
            };

            _textBlock.MouseUp += (sender, e) =>
            {
                if (_textBlock.IsMouseCaptured)
                    _textBlock.ReleaseMouseCapture();

                if (!mouseMoved)
                {
                    Mode = NumericBoxMode.TextBox;
                    _textBox.SelectAll();
                    _textBox.Focus();
                }
            };

            _textBox = (TextBox)Template.FindName("TextBox", this);
            _textBox.KeyUp += (sender, e) =>
            {
                if (e.Key == Key.Escape || e.Key == Key.Enter)
                    Mode = NumericBoxMode.Normal;
            };
            _textBox.LostFocus += (sender, e) => Mode = NumericBoxMode.Normal;

            base.OnApplyTemplate();
        }

        private double CoerceValue(double newValue)
        {
            if (Minimum != null && newValue < Minimum.Value)
                return Minimum.Value;
            if (Maximum != null && newValue > Maximum.Value)
                return Maximum.Value;
            return newValue;
        }
    }

    public enum NumericBoxMode
    {
        Normal,
        TextBox
    }
}
