using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfEditor.Editor.ViewModels;
using TrioWpfFramework.ServiceLocation;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.ScreenConduction;

namespace TrioWpfFramework.Editor.Services
{
    public interface IEditorService : IDockControl, IScreenConductor, IScreen, IActivatable, IGuardClose, IDisplayName
    {
        string ApplicationName { get; set; }

        object ApplicationIcon { get; set; }

        string Subtitle { get; set; }

        int ExitCode { get; }

        ServiceContainer Services { get; }

        event EventHandler<EventArgs> UIInvalidated;

        void InvalidateUI();

        event EventHandler<EventArgs> LayoutChanged;

        event EventHandler<EventArgs> ActiveDockTabItemChanged;

        new IEnumerable<EditorDockTabItemViewModel> Items { get; }

        void ActivateItem(object item);
    }
}
