using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public interface IDockAnchorPane : IDockPane
    {
        /// <summary>
        /// Gets or sets the child pane.
        /// </summary>
        /// <value>The child pane.</value>
        IDockPane ChildPane { get; set; }
    }
}
