using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Delivery.ViewModels.Core;

namespace TrioWpfFramework.Editor.Commands.Toolbars
{
    /// <summary>
    /// Represents a command item in a toolbar.
    /// </summary>
    public abstract class ToolBarItemViewModel : ObservableObject
    {
        /// <summary>
        /// Gets or sets the command item.
        /// </summary>
        /// <value>The command item.</value>
        public ICommandItem CommandItem { get; }


        /// <summary>
        /// Initializes a new instance of the <see cref="ToolBarItemViewModel"/> class.
        /// </summary>
        /// <param name="commandItem">The command item.  Must not be <see langword="null"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandItem"/> is <see langword="null"/>.
        /// </exception>
        protected ToolBarItemViewModel(ICommandItem commandItem)
        {
            if (commandItem == null)
                throw new ArgumentNullException(nameof(commandItem));

            CommandItem = commandItem;
        }
    }
}
