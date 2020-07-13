using Estero.Linq;
using System;
using System.Windows;
using System.Windows.Media;

namespace EsteroWindows
{
    public static class WpfExtensions
    {
        public static DependencyObject GetVisualRoot(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetRoot(dependencyObject, VisualTreeHelper.GetParent);
        }
    }
}
