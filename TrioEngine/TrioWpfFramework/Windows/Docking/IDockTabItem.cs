using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking
{
    public interface IDockTabItem : IDockElement
    {
        string DockId { get; }

        object Icon { get; }

        string Title { get; }

        bool IsPersistent { get; }

        double AutoHideWidth { get; set; }

        double AutoHideHeight { get; set; }

        DockState LastDockState { get; set; }

        DateTime LastActivation { get; set; }

    }
}
