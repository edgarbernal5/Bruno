using Microsoft.Xaml.Behaviors;
using System;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Navigation;

namespace BrunoFramework
{
    public class WindowManager : IWindowManager
    {
        private IViewLocator _viewLocator;
        public WindowManager(IViewLocator viewLocator)
        {
            _viewLocator = viewLocator;
        }

        public virtual async Task<bool?> ShowDialog(object viewModel, object context = null)
        {
            if (viewModel == null)
                throw new ArgumentNullException(nameof(viewModel));

            var window = await CreateWindowAsync(viewModel, context, asChildWindow: true, asDialog: true);
            return window.ShowDialog();
        }

        public async Task ShowWindow(object viewModel, object context = null, bool asChildWindow = true)
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
                var window = await CreateWindowAsync(viewModel, context, asChildWindow, false);
                window.Show();
            }
        }

        protected virtual async Task<Window> CreateWindowAsync(object viewModel, object context, bool asChildWindow, bool asDialog)
        {
            var view = _viewLocator.GetView(viewModel, null, context);
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
            await windowConductor.InitializeAsync();

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

            var hasDialogResult = viewModel as IDialogResult;
            if (asDialog && hasDialogResult != null)
            {
                var behaviors = Interaction.GetBehaviors(window);
                if (!behaviors.OfType<DialogResultBehavior>().Any())
                {
                    var dialogResultBehavior = new DialogResultBehavior();
                    var binding = new Binding("DialogResult") { Mode = BindingMode.TwoWay };
                    BindingOperations.SetBinding(dialogResultBehavior, DialogResultBehavior.DialogResultProperty, binding);
                    behaviors.Add(dialogResultBehavior);
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
