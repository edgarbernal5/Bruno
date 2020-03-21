using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using TrioWpfFramework.Windows.Docking.Controls;
using TrioWpfFramework.Windows.Interop;

namespace TrioWpfFramework.Windows.Docking
{
    public class AutoHideOverlay : DockOverlay
    {
        public DockControl DockControl { get; }

        static AutoHideOverlay()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(AutoHideOverlay), new FrameworkPropertyMetadata(typeof(AutoHideOverlay)));
        }

        public AutoHideOverlay(DockControl dockControl, FrameworkElement target)
              : base(target)
        {
            DockControl = dockControl;
            Owner = GetWindow(target);
            SourceInitialized += OnSourceInitialized;

            Loaded += OnLoaded;
            Unloaded += OnUnloaded;
        }

        private void OnSourceInitialized(object sender, EventArgs eventArgs)
        {
            var helper = new WindowInteropHelper(this);

            // Make the overlay window a child of the main window. Otherwise the overlay window
            // steals the focus from the main window.
            int oldWindowStyle = Win32.GetWindowLong(helper.Handle, GetWindowLongIndex.GWL_STYLE);
            int newWindowStyle = oldWindowStyle | (int)WindowStyles.WS_CHILD;
            Win32.SetWindowLong(helper.Handle, GetWindowLongIndex.GWL_STYLE, newWindowStyle);

            // The flag WS_EX_NOACTIVATE is necessary, otherwise the child window cannot receive
            // keyboard focus.
            int oldWindowStyleEx = Win32.GetWindowLong(helper.Handle, GetWindowLongIndex.GWL_EXSTYLE);
            int newWindowStyleEx = oldWindowStyleEx | (int)WindowStylesEx.WS_EX_NOACTIVATE;
            Win32.SetWindowLong(helper.Handle, GetWindowLongIndex.GWL_EXSTYLE, newWindowStyleEx);
        }

        private void OnLoaded(object sender, RoutedEventArgs eventArgs)
        {
            DockControl?._autoHideOverlays.Add(this);
        }

        private void OnUnloaded(object sender, RoutedEventArgs eventArgs)
        {
            DockControl?._autoHideOverlays.Remove(this);
        }
    }
}
