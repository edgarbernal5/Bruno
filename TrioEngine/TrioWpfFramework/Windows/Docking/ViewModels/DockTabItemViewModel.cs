using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public class DockTabItemViewModel : DockElementViewModel, IDockTabItem
    {
        public DockState LastDockState
        {
            get { return _lastDockState; }
            set { SetProperty(ref _lastDockState, value); }
        }
        private DockState _lastDockState;

        public bool IsPersistent
        {
            get { return _isPersistent; }
            set { SetProperty(ref _isPersistent, value); }
        }
        private bool _isPersistent;

        public DateTime LastActivation
        {
            get { return _lastActivation; }
            set { SetProperty(ref _lastActivation, value); }
        }
        private DateTime _lastActivation;

        public double AutoHideWidth
        {
            get { return _autoHideWidth; }
            set { SetProperty(ref _autoHideWidth, value); }
        }
        private double _autoHideWidth = double.NaN;

        public double AutoHideHeight
        {
            get { return _autoHideHeight; }
            set { SetProperty(ref _autoHideHeight, value); }
        }
        private double _autoHideHeight = double.NaN;

        public object Icon
        {
            get { return _icon; }
            set { SetProperty(ref _icon, value); }
        }
        private object _icon;

        public string Title
        {
            get { return _title; }
            set { SetProperty(ref _title, value); }
        }
        private string _title;

        public string DockId
        {
            get { return _dockId; }
            set { SetProperty(ref _dockId, value); }
        }
        private string _dockId;
    }
}
