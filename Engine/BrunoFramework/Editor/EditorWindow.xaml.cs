
using BrunoWindows.Interop;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace BrunoFramework.Editor
{
    /// <summary>
    /// Interaction logic for EditorWindow.xaml
    /// </summary>
    public partial class EditorWindow : Window
    {
        private EditorViewModel m_editorViewModel;

        public EditorWindow()
        {
            InitializeComponent();

            DataContextChanged += OnDataContextChanged;
            Loaded += OnLoaded;
            Unloaded += OnUnloaded;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            var editorViewModel = DataContext as EditorViewModel;

            if (m_editorViewModel != null)
            {
                m_editorViewModel.Window = null;
                m_editorViewModel.DockManager = null;
            }

            m_editorViewModel = editorViewModel;

            if (editorViewModel != null)
            {
                m_editorViewModel.Window = this;
                m_editorViewModel.DockManager = DockManager;
            }
        }

        private void OnLoaded(object sender, RoutedEventArgs eventArgs)
        {
        }

        private void OnUnloaded(object sender, RoutedEventArgs eventArgs)
        {
            m_editorViewModel.Window = null;
            m_editorViewModel.DockManager = null;
        }

        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            //var hook = new HwndSourceHook(FilterMessage);
            //var hwndSource = (HwndSource)PresentationSource.FromVisual(this);
            //Debug.Assert(hwndSource != null, "Unable to retrieve HWND of main window.");
            //hwndSource.AddHook(hook);
        }

        private void OnPreviewMenuGotKeyboardFocus(object sender, KeyboardFocusChangedEventArgs eventArgs)
        {
            if (!IsActive)
            {
                // Set the event to handled. This prevents WPF from activating the window due to
                // the mouse click on the menu.
                // The disadvantage is that now the menu cannot be controlled with the keyboard
                // (e.g. up/down arrow keys). Therefore, we do this only when a different window
                // is active. That means, if the editor window has focus everything is normal. If
                // a float window has focus, then we cannot use the window with the keyboard, but
                // commands are properly routed from the menu to the float window.
                // (A better solution could be this:
                // https://blogs.msdn.microsoft.com/visualstudio/2010/03/08/wpf-in-visual-studio-2010-part-3-focus-and-activation/
                // But I did not find enough information to make it work. Perhaps this concerns only
                // Win32 content in a float window.
                // A different solution would be: Do not set Handled. Instead, remember the focused
                // element in the float window. Route commands to this element. Then set focus back
                // to this element when a menu command is executed or a top menu item is closed
                // using mouse click or escape key.)
                eventArgs.Handled = true;
            }
        }
    }
}
