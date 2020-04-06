
using System.Windows;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public abstract class DockElementViewModel : ObservableObject, IDockElement
    {
        public DockState DockState
        {
            get { return _dockState; }
            set { SetProperty(ref _dockState, value); }
        }
        private DockState _dockState;

        public GridLength DockWidth
        {
            get { return _dockWidth; }
            set { SetProperty(ref _dockWidth, value); }
        }
        private GridLength _dockWidth = new GridLength(1, GridUnitType.Star);

        public GridLength DockHeight
        {
            get { return _dockHeight; }
            set { SetProperty(ref _dockHeight, value); }
        }
        private GridLength _dockHeight = new GridLength(1, GridUnitType.Star);
    }
}
