using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor.Commands.Menu
{
    /// <summary>
    /// Represents a separator in a menu.
    /// </summary>
    internal sealed class MenuSeparatorViewModel : MenuItemViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="MenuSeparatorViewModel"/> class.
        /// </summary>
        /// <param name="commandSeparator">
        /// The command separator. Must not be <see langword="null"/>.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandSeparator"/> is <see langword="null"/>.
        /// </exception>
        public MenuSeparatorViewModel(CommandSeparator commandSeparator)
            : base(commandSeparator)
        {
        }
    }
}
