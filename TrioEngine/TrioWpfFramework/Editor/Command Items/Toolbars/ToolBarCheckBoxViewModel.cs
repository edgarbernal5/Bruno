using System;
using System.Diagnostics;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Represents a check box in a toolbar.
    /// </summary>
    public sealed class ToolBarCheckBoxViewModel : ToolBarItemViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ToolBarCheckBoxViewModel"/> class.
        /// </summary>
        /// <param name="commandItem">The command item. Must not be <see langword="null"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandItem"/> is <see langword="null"/>.
        /// </exception>
        public ToolBarCheckBoxViewModel(CommandItem commandItem) : base(commandItem)
        {
            Debug.Assert(commandItem.IsCheckable, "ToolBarCheckBoxViewModel should be used with checkable command items.");
        }
    }
}