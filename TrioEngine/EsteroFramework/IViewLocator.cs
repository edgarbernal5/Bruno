
using System.Windows;

namespace EsteroFramework
{
    public interface IViewLocator
    {
        /// <summary>
        /// Resolves the view for the specified view model in the current context.
        /// </summary>
        FrameworkElement GetView(object viewModel, DependencyObject parent = null, object context = null);
    }
}
