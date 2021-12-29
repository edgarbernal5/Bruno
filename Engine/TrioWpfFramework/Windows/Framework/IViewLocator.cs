using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TrioWpfFramework.Windows.Framework
{
    /// <summary>
    /// A strategy for determining which view to use for a specific view model.
    /// </summary>
    public interface IViewLocator
    {
        /// <summary>
        /// Resolves the view for the specified view model in the current context.
        /// </summary>
        /// <param name="viewModel">The view model.</param>
        /// <param name="parent">The parent control. Can be <see langword="null"/>.</param>
        /// <param name="context">The context. Can be <see langword="null"/>.</param>
        /// <returns>
        /// The view or <see langword="null"/> if no matching view is found.
        /// </returns>
        FrameworkElement GetView(object viewModel, DependencyObject parent = null, object context = null);

    }
}
