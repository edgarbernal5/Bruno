
using System.Windows;

namespace EsteroFramework
{
    public interface IViewLocator
    {
        FrameworkElement GetView(object viewModel, DependencyObject parent = null, object context = null);
    }
}
