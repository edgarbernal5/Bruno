
using System.Windows;

namespace BrunoFramework
{
    public interface IViewLocator
    {
        FrameworkElement GetView(object viewModel, DependencyObject parent = null, object context = null);
    }
}
