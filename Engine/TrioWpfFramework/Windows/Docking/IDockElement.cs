using System.Windows;

namespace TrioWpfFramework.Windows.Docking
{
    public interface IDockElement
    {
        DockState DockState { get; set; }

        GridLength DockWidth { get; set; }

        GridLength DockHeight { get; set; }
    }
}
