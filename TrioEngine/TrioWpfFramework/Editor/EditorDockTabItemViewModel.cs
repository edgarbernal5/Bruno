using System;
using System.ComponentModel;
using System.Windows;
using TrioWpfFramework.Editor;
using TrioWpfFramework.Windows.Docking;
using TrioWpfFramework.Windows.Framework;

namespace TrioWpfFramework.Editor
{
    public abstract class EditorDockTabItemViewModel : Screen, IDockTabItem
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

        public DockState LastDockState
        {
            get { return _lastDockState; }
            set { SetProperty(ref _lastDockState, value); }
        }
        private DockState _lastDockState;

        public bool IsPersistent
        {
            get { return _isPersistent; }
            protected set { SetProperty(ref _isPersistent, value); }
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
            get { return DisplayName; }
        }

        public string DockId
        {
            get { return _dockId; }
            protected set { SetProperty(ref _dockId, value); }
        }
        private string _dockId;

        public MenuItemViewModelCollection DockContextMenu
        {
            get { return _dockContextMenu; }
            set { SetProperty(ref _dockContextMenu, value); }
        }
        private MenuItemViewModelCollection _dockContextMenu;


        public object DockToolTip
        {
            get { return _dockToolTip; }
            set { SetProperty(ref _dockToolTip, value); }
        }
        private object _dockToolTip;

        protected override void OnPropertyChanged(PropertyChangedEventArgs eventArgs)
        {
            // Property Title is the same as DisplayName. --> Raise Title changed event when
            // DisplayName changes.
            if (string.IsNullOrEmpty(eventArgs.PropertyName) || nameof(DisplayName) == eventArgs.PropertyName)
            {
                // ReSharper disable once ExplicitCallerInfoArgument
                RaisePropertyChanged(nameof(IDockTabItem.Title));
            }

            base.OnPropertyChanged(eventArgs);
        }

        protected override void OnActivated(ActivationEventArgs eventArgs)
        {
            //Logger.Debug("Activating {0} (\"{1}\").", GetType().Name, DisplayName);

            // If the user does not set a context menu, we use the default context menu defined
            // in the editor.
            if (DockContextMenu == null)
                DockContextMenu = (Conductor as EditorViewModel)?.DockContextMenu;

            base.OnActivated(eventArgs);
        }


        /// <inheritdoc/>
        protected override void OnDeactivated(DeactivationEventArgs eventArgs)
        {
            //Logger.Debug("Deactivating {0} (\"{1}\").", GetType().Name, DisplayName);

            base.OnDeactivated(eventArgs);
        }
    }
}
