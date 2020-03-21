using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.ScreenConduction
{
    public interface IScreen
    {
        IScreenConductor Conductor { get; set; }
    }
}
