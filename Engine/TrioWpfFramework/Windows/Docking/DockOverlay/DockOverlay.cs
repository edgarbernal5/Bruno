using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TrioWpfFramework.Windows.Docking
{
    /// <summary>
    /// Represents an overlay that is shown over a target element.
    /// </summary>
    /// <remarks>
    /// The overlay window is a transparent window that has no visible border or style. It is not a
    /// activated when shown, nor does it appear in the Windows taskbar. It covers a certain target
    /// element (see property <see cref="Target"/>). When the target element is resized or moved,
    /// the overlay automatically adjusts its size and position.
    /// </remarks>
    public abstract class DockOverlay : Window
    {
        private FrameworkElement _targetElement;
        private Window _targetWindow;

        public FrameworkElement Target
        {
            get { return _targetElement; }
            set
            {
                if (value == null)
                    throw new ArgumentNullException(nameof(value));
                if (_targetElement == value)
                    return;

                DetachFromTarget();
                _targetElement = value;
                AttachToTarget();
                UpdateBounds();
            }
        }

        static DockOverlay()
        {
            AllowsTransparencyProperty.OverrideMetadata(typeof(DockOverlay), new FrameworkPropertyMetadata(Boxed.BooleanTrue));
            BackgroundProperty.OverrideMetadata(typeof(DockOverlay), new FrameworkPropertyMetadata(null));
            ShowActivatedProperty.OverrideMetadata(typeof(DockOverlay), new FrameworkPropertyMetadata(Boxed.BooleanFalse));
            ShowInTaskbarProperty.OverrideMetadata(typeof(DockOverlay), new FrameworkPropertyMetadata(Boxed.BooleanFalse));
            WindowStyleProperty.OverrideMetadata(typeof(DockOverlay), new FrameworkPropertyMetadata(WindowStyle.None));
        }
        protected DockOverlay(FrameworkElement target)
        {
            if (target == null)
                throw new ArgumentNullException(nameof(target));

            Target = target;
            Loaded += OnLoaded;
        }

        private void AttachToTarget()
        {
            if (_targetElement == null)
                return;

            Debug.Assert(_targetElement != null);
            Debug.Assert(_targetWindow == null);

            _targetWindow = GetWindow(_targetElement);
            if (_targetWindow == null)
                throw new DockException("Parent window of target element not found.");

            _targetWindow.LocationChanged += OnTargetChanged;
            _targetWindow.SizeChanged += OnTargetChanged;
            _targetElement.SizeChanged += OnTargetChanged;
        }


        private void DetachFromTarget()
        {
            if (_targetElement == null)
                return;

            Debug.Assert(_targetElement != null);
            Debug.Assert(_targetWindow != null);

            _targetWindow.LocationChanged -= OnTargetChanged;
            _targetWindow.SizeChanged -= OnTargetChanged;
            _targetElement.SizeChanged -= OnTargetChanged;

            _targetWindow = null;
        }


        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            UpdateBounds();
        }


        private void OnTargetChanged(object sender, EventArgs eventArgs)
        {
            UpdateBounds();
        }


        private void UpdateBounds()
        {
            // TargetElement sometimes becomes null inside this method. Not sure how or why. -- MartinG
            // --> Copy the reference first and work with copy!
            var targetElement = _targetElement;
            if (targetElement != null && targetElement.IsVisible)
            {
                Point position = targetElement.PointToScreen(new Point(0, 0));
                position = DockHelper.ScreenToLogical(this, position);

                Left = position.X;
                Top = position.Y;
                Width = targetElement.ActualWidth;
                Height = targetElement.ActualHeight;
            }
        }


        /// <summary>
        /// Raises the <see cref="Window.Closed"/> event.
        /// </summary>
        /// <param name="e">An <see cref="EventArgs"/> that contains the event data.</param>
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            DetachFromTarget();
        }
    }
}
