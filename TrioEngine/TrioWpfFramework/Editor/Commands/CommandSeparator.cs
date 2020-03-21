using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using TrioWpfFramework.Delivery.ViewModels.Core;
using TrioWpfFramework.Editor.Commands.Menu;
using TrioWpfFramework.Editor.Commands.Toolbars;

namespace TrioWpfFramework.Editor.Commands
{
    /// <summary>
    /// Represents a separator between command items.
    /// </summary>
    public sealed class CommandSeparator : ObservableObject, ICommandItem
    {
        // Note: Properties are public to enable data binding. Properties that are NEVER used in
        // data bindings are implicit interface implementations.

        public string Name { get; }


        bool ICommandItem.AlwaysShowText { get { return false; } }

        string ICommandItem.Category { get { return null; } }

        ICommand ICommandItem.Command { get { return null; } }

        object ICommandItem.CommandParameter { get { return null; } }

        object ICommandItem.Icon { get { return null; } }


        /// <inheritdoc/>
        InputGestureCollection ICommandItem.InputGestures { get { return null; } }


        /// <inheritdoc/>
        bool ICommandItem.IsCheckable { get { return false; } }


        /// <inheritdoc/>
        bool ICommandItem.IsChecked { get { return false; } }


        /// <inheritdoc/>
        string ICommandItem.Text { get { return null; } }


        /// <inheritdoc/>
        string ICommandItem.ToolTip { get { return null; } }


        /// <inheritdoc/>
        bool ICommandItem.IsVisible
        {
            // Separators are always visible. However, the visibility of the linked menu item or
            // toolbar item is set automatically depending on the neighbor items.
            get { return true; }
            set { throw new NotSupportedException(); }
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="CommandSeparator"/> class.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="name"/> is <see langword="null"/>.
        /// </exception>
        public CommandSeparator(string name)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));

            Name = name;
        }

        public MenuItemViewModel CreateMenuItem()
        {
            return new MenuSeparatorViewModel(this);
        }


        /// <inheritdoc/>
        public ToolBarItemViewModel CreateToolBarItem()
        {
            return new ToolBarSeparatorViewModel(this);
        }
    }
}
