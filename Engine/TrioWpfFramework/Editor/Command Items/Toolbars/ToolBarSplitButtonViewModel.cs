using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Represents split button in a toolbar.
    /// </summary>
    public class ToolBarSplitButtonViewModel : ToolBarDropDownButtonViewModel
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ToolBarSplitButtonViewModel"/> class.
        /// </summary>
        /// <param name="commandItem">The command item. Must not be <see langword="null"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandItem"/> is <see langword="null"/>.
        /// </exception>
        public ToolBarSplitButtonViewModel(ICommandItem commandItem)
            : base(commandItem)
        {
        }
    }
}
