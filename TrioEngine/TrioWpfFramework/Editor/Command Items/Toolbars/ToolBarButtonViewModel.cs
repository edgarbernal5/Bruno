using System;
using System.Diagnostics;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Represents a button in a toolbar.
    /// </summary>
    public sealed class ToolBarButtonViewModel : ToolBarItemViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ToolBarButtonViewModel"/> class.
        /// </summary>
        /// <param name="commandItem">The command item. Must not be <see langword="null"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandItem"/> is <see langword="null"/>.
        /// </exception>
        public ToolBarButtonViewModel(CommandItem commandItem) : base(commandItem)
        {
            Debug.Assert(!commandItem.IsCheckable, "ToolBarButtonViewModel should not be used with checkable command items.");
        }
    }
}
