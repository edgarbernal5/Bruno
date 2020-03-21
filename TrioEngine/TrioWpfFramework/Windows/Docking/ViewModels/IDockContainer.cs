using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public interface IDockContainer
    {
        IDockPane RootPane { get; set; }
    }
}
