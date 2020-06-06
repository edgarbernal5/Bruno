using System;
using System.Linq;
using System.Windows;
using System.Windows.Data;
using System.Windows.Navigation;

namespace EsteroFramework
{
    public class WindowManager : IWindowManager
    {
        private IViewLocator _viewLocator;
        public WindowManager(IViewLocator viewLocator)
        {
            _viewLocator = viewLocator;
        }

        public bool? ShowDialog(object viewModel, object context = null)
        {
            if (viewModel == null)
                throw new ArgumentNullException(nameof(viewModel));

            var window = CreateWindow(viewModel, context, asChildWindow: true, asDialog: true);
            return window.ShowDialog();
        }

        public void ShowWindow(object viewModel, object context = null, bool asChildWindow = true)
        {
            if (viewModel == null)
                throw new ArgumentNullException(nameof(viewModel));

            NavigationWindow navWindow = null;

            var application = Application.Current;
            if (application != null && application.MainWindow != null)
            {
                navWindow = application.MainWindow as NavigationWindow;
            }

            if (navWindow != null)
            {
                throw new NotImplementedException();
            }
            else
            {
                var window = CreateWindow(viewModel, context, asChildWindow, false);
                window.Show();
            }
        }

        protected virtual Window CreateWindow(object viewModel, object context, bool asChildWindow, bool asDialog)
        {
            var view = _viewLocator?.GetView(viewModel, null, context);
            var window = EnsureWindow(viewModel, view, asChildWindow, asDialog);
            window.DataContext = viewModel;

            var haveDisplayName = viewModel as IHaveDisplayName;
            if (string.IsNullOrEmpty(window.Title) && haveDisplayName != null &&
                BindingOperations.GetBinding(window, Window.TitleProperty) == null)
            {
                var binding = new Binding("DisplayName") { Mode = BindingMode.TwoWay };
                view.SetBinding(Window.TitleProperty, binding);
            }

            var windowConductor = new WindowConductor(viewModel, window);
            windowConductor.Initialize();

            return window;
        }

        private Window EnsureWindow(object viewModel, object view, bool asChildWindow, bool asDialog)
        {
            var window = view as Window;
            if (window == null)
            {
                window = new Window
                {
                    Content = view ?? viewModel,
                    SizeToContent = SizeToContent.WidthAndHeight,
                };
                //window.Loaded += (sender, e) => window.MoveFocus(new TraversalRequest(FocusNavigationDirection.Next));

                if (asDialog)
                {
                    window.ResizeMode = ResizeMode.NoResize;

                    // According to Windows Style Guides: Model dialog should not have system menu.
                    //WindowsHelper.SetShowIcon(window, false);
                }

                var owner = InferOwnerOf(window);
                if (owner != null)
                {
                    window.WindowStartupLocation = WindowStartupLocation.CenterOwner;
                    window.Owner = owner;
                }
                else
                {
                    window.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                }
            }
            else
            {
                var owner = InferOwnerOf(window);
                if (owner != null && asChildWindow)
                {
                    window.ShowInTaskbar = false;
                    window.WindowStartupLocation = WindowStartupLocation.CenterOwner;
                    window.Owner = owner;
                }
                else
                {
                    window.WindowStartupLocation = WindowStartupLocation.CenterScreen;
                }
            }

            return window;
        }

        protected virtual Window InferOwnerOf(Window window)
        {
            var application = Application.Current;
            if (application == null)
            {
                return null;
            }

            var activeWindow = application.Windows.OfType<Window>().FirstOrDefault(x => x.IsActive);
            activeWindow = activeWindow ?? (PresentationSource.FromVisual(application.MainWindow) == null ? null : application.MainWindow);
            return activeWindow == window ? null : activeWindow;
        }
    }
}
