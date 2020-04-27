using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Framework
{
    /// <summary>
    /// Manages windows and dialog boxes.
    /// </summary>
    public interface IWindowService
    {
        /// <summary>
        /// Shows a modal dialog for the specified view model.
        /// </summary>
        /// <param name="viewModel">The view model.</param>
        /// <param name="context">
        /// The context. This object will be given to the <see cref="IViewLocator"/>.
        /// </param>
        /// <returns>
        /// A <see cref="Nullable{T}"/> value of type <see cref="bool"/> that specifies whether the
        /// dialog was accepted (true) or canceled (false). The return value is the value of the
        /// <strong>DialogResult</strong> property before the window closes.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="viewModel"/> is <see langword="null"/>.
        /// </exception>
        bool? ShowDialog(object viewModel, object context = null);

        /// <summary>
        /// Shows a non-modal window for the specified view model.
        /// </summary>
        /// <param name="viewModel">The view model.</param>
        /// <param name="context">
        /// The context. This object will be given to the <see cref="IViewLocator"/>.
        /// </param>
        /// <param name="asChildWindow">
        /// <see langword="true"/> if the window is a child window; otherwise,
        /// <see langword="false"/>.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="viewModel"/> is <see langword="null"/>.
        /// </exception>
        void ShowWindow(object viewModel, object context = null, bool asChildWindow = true);
    }
}
