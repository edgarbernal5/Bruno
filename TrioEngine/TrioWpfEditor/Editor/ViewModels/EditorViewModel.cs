using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using TrioWpfFramework.Net.Linq;
using TrioWpfFramework.Editor.Services;
using TrioWpfFramework.ServiceLocation;
using TrioWpfFramework.Windows.Docking;
using TrioWpfFramework.Windows.Docking.Controls;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.ScreenConduction;
using static System.FormattableString;
using TrioWpfFramework.Editor;
using TrioWpfFramework.Editor.Commands;

namespace TrioWpfEditor.Editor.ViewModels
{
    public class EditorViewModel : Screen, IEditorService
    {
        private bool _isInitialized;
        private ResourceDictionary _resourceDictionary;

        public string ApplicationName
        {
            get { return _applicationName; }
            set
            {
                if (SetProperty(ref _applicationName, value))
                {
                    UpdateTitle();
                }
            }
        }
        private string _applicationName;


        public object ApplicationIcon
        {
            get { return _applicationIcon; }
            set { SetProperty(ref _applicationIcon, value); }
        }
        private object _applicationIcon;

        public DockStrategy DockStrategy
        {
            get { return _dockStrategy; }
            set
            {
                if (_dockStrategy == value)
                    return;

                if (_dockStrategy != null)
                    _dockStrategy.DockControl = null;

                _dockStrategy = value;

                if (_dockStrategy != null)
                    _dockStrategy.DockControl = this;

                RaisePropertyChanged();
            }
        }
        private DockStrategy _dockStrategy;

        public string Subtitle
        {
            get { return _subtitle; }
            set
            {
                if (SetProperty(ref _subtitle, value))
                {
                    UpdateTitle();
                }
            }
        }
        private string _subtitle;

        public int ExitCode
        {
            get { return _exitCode ?? (int)TrioWpfFramework.Editor.ExitCode.ERROR_SUCCESS; }
        }
        private int? _exitCode;

        public ServiceContainer Services { get; }

        public EditorPluginCollection Extensions { get; }
        private IEnumerable<EditorPlugin> OrderedExtensions
        {
            get
            {
                // Note: OrderByDescending is a stable sort.
                return Extensions.OrderByDescending(e => e.Priority);
            }
        }

        public EditorWindow Window
        {
            get { return _window; }
            internal set
            {
                if (_window == value)
                    return;

                if (_window != null)
                    RemoveInputBindings(_window);

                _window = value;

                // Update input/command bindings but only if editor IsOpen is set. If not IsOpen
                // UpdateInputAndCommandBindings will be called in OnActivated.
                if (_window != null && IsOpen)
                    UpdateInputAndCommandBindings();
            }
        }
        private EditorWindow _window;

        /// <summary>
        /// Gets the command that is executed when the window becomes the foreground window.
        /// </summary>
        /// <value>
        /// The command that is executed when the window becomes the foreground window.
        /// </value>
        public ICommand WindowActivationCommand { get; }

        public event EventHandler<EventArgs> WindowActivated;

        public IDockPane RootPane
        {
            get { return _rootPane; }
            set { SetProperty(ref _rootPane, value); }
        }
        private IDockPane _rootPane = new DockAnchorPaneViewModel();

        public IDockTabPane ActiveDockTabPane
        {
            get { return _activeDockTabPane; }
            set { SetProperty(ref _activeDockTabPane, value); }
        }
        private IDockTabPane _activeDockTabPane;

        public bool IsLocked
        {
            get { return _isLocked; }
            set { SetProperty(ref _isLocked, value); }
        }
        private bool _isLocked;
        public IDockTabItem ActiveDockTabItem
        {
            get { return _activeDockTabItem; }
            set
            {
                if (SetProperty(ref _activeDockTabItem, value))
                    OnActiveDockTabItemChanged(EventArgs.Empty);
            }
        }
        private IDockTabItem _activeDockTabItem;

        public FloatWindowCollection FloatWindows { get; } = new FloatWindowCollection();

        public DockTabPaneCollection AutoHideLeft { get; } = new DockTabPaneCollection();

        public DockTabPaneCollection AutoHideRight { get; } = new DockTabPaneCollection();


        public DockTabPaneCollection AutoHideTop { get; } = new DockTabPaneCollection();

        public DockTabPaneCollection AutoHideBottom { get; } = new DockTabPaneCollection();

        public DockControl DockControl
        {
            get { return _dockControl; }
            set
            {
                if (_dockControl == value)
                    return;

                if (_dockControl != null)
                {
                    ((INotifyCollectionChanged)_dockControl.FloatWindows).CollectionChanged -= OnFloatWindowsChanged;
                    ((INotifyCollectionChanged)_dockControl.AutoHideOverlays).CollectionChanged -= OnAutoHideOverlaysChanged;
                    RemoveInputBindings(_dockControl);
                    RemoveCommandBindings(_dockControl);
                }

                _dockControl = value;

                if (_dockControl != null)
                {
                    AddInputBindings(_dockControl);
                    AddCommandBindings(_dockControl);
                    ((INotifyCollectionChanged)_dockControl.FloatWindows).CollectionChanged += OnFloatWindowsChanged;
                    ((INotifyCollectionChanged)_dockControl.AutoHideOverlays).CollectionChanged += OnAutoHideOverlaysChanged;
                }
            }
        }

        private DockControl _dockControl;

        public event EventHandler<EventArgs> LayoutChanged;

        public event EventHandler<EventArgs> ActiveDockTabItemChanged;
        public event EventHandler<EventArgs> UIInvalidated;

        // Original input bindings of the main window.
        private readonly List<InputBinding> _inputBindings = new List<InputBinding>();

        // Input bindings created from command item.
        private readonly List<InputBinding> _commandItemInputBindings = new List<InputBinding>();

        // Original command bindings of the main window.
        private readonly List<CommandBinding> _commandBindings = new List<CommandBinding>();

        // Merge DockTabItem context menu nodes and create the menu items. (Note: The managers need
        // to be kept alive because they manage the visibility of the items but are weak event
        // listeners.)
        internal MenuManager MenuManager;       // Used in EditorWindow.
        internal ToolBarManager ToolBarManager; // Used in EditorWindow.
        private MenuManager _contextMenuManager;

        IEnumerable<object> IScreenConductor.Items
        {
            get { return GetItems(false); }
        }
        IEnumerable<object> IScreenConductor.ActiveItems
        {
            get { return GetItems(true); }
        }

        public IEnumerable<EditorDockTabItemViewModel> Items
        {
            get { return GetItems(false); }
        }

        public MenuItemViewModelCollection DockContextMenu
        {
            get { return _contextMenuManager.Menu; }
        }


        public EditorViewModel(ServiceContainer serviceContainer)
        {
            //Logger.Debug("Creating EditorViewModel.");

            if (serviceContainer == null)
                throw new ArgumentNullException(nameof(serviceContainer));

            Services = serviceContainer;

            DockStrategy = new EditorDockStrategy();

            _applicationName = "TrioEditor"; // EditorHelper.GetDefaultApplicationName();
            _subtitle = null;

            Extensions = new EditorPluginCollection();

            InitializeCommandItems();

            WindowActivationCommand = new DelegateCommand(() => OnWindowActivated(EventArgs.Empty));
        }

        private void OnFloatWindowsChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            if (eventArgs.Action == NotifyCollectionChangedAction.Add)
            {
                foreach (var window in eventArgs.NewItems.Cast<FloatWindow>())
                {
                    AddInputBindings(window);
                    AddCommandBindings(window);
                }
            }
            else if (eventArgs.Action == NotifyCollectionChangedAction.Reset)
            {
                foreach (var window in DockControl.FloatWindows)
                {
                    // Clean up to avoid duplicates.
                    RemoveInputBindings(window);
                    RemoveCommandBindings(window);

                    AddInputBindings(window);
                    AddCommandBindings(window);
                }
            }
        }


        private void OnAutoHideOverlaysChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            if (eventArgs.Action == NotifyCollectionChangedAction.Add)
            {
                foreach (var window in eventArgs.NewItems.Cast<AutoHideOverlay>())
                {
                    AddInputBindings(window);
                    AddCommandBindings(window);
                }
            }
            else if (eventArgs.Action == NotifyCollectionChangedAction.Reset)
            {
                foreach (var window in DockControl.AutoHideOverlays)
                {
                    // Clean up to avoid duplicates.
                    RemoveInputBindings(window);
                    RemoveCommandBindings(window);

                    AddInputBindings(window);
                    AddCommandBindings(window);
                }
            }
        }

        protected internal virtual void OnLayoutChanged(EventArgs eventArgs)    // Called in EditorDockStrategy.
        {
            LayoutChanged?.Invoke(this, eventArgs);
        }

        protected override void OnActivated(ActivationEventArgs eventArgs)
        {
            base.OnActivated(eventArgs);

            // Activate all selected items.
            this.GetDockElements()
                .OfType<IDockTabPane>()
                .Where(dockTabPane => dockTabPane.IsVisible)
                .Select(dockTabPane => dockTabPane.SelectedItem)
                .OfType<IActivatable>()
                .ForEach(activatable => activatable.OnActivate());

            if (eventArgs.Opened)
            {
                // Note: Some extensions register command bindings when in 
                // EditorViewModel.Activated. Therefore, base.OnActivate must be called before this.
                UpdateInputAndCommandBindings();
            }
        }

        protected override void OnDeactivated(DeactivationEventArgs eventArgs)
        {
            if (eventArgs.Closed)
            {
                // Close all items.
                var dockTabItems = this.GetDockElements()
                                       .OfType<IDockTabItem>()
                                       .Distinct();
                foreach (var dockTabItem in dockTabItems)
                {
                    (dockTabItem as IActivatable)?.OnDeactivate(true);

                    var screen = dockTabItem as IScreen;
                    if (screen != null)
                        screen.Conductor = null;
                }
            }
            else
            {
                // Deactivate all selected items.
                this.GetDockElements()
                    .OfType<IDockTabPane>()
                    .Where(dockTabPane => dockTabPane.IsVisible)
                    .Select(dockTabPane => dockTabPane.SelectedItem)
                    .OfType<IActivatable>()
                    .ForEach(activatable => activatable.OnDeactivate(false));
            }

            base.OnDeactivated(eventArgs);
        }

        public override async Task<bool> CanCloseAsync()
        {
            foreach (var extension in OrderedExtensions.OfType<IGuardClose>())
            {
                bool canClose = await extension.CanCloseAsync();
                if (!canClose)
                    return false;
            }

            foreach (var item in Items)
            {
                bool canClose = await item.CanCloseAsync();
                if (!canClose)
                    return false;
            }

            return await base.CanCloseAsync();
        }

        protected virtual void OnActiveDockTabItemChanged(EventArgs eventArgs)
        {
            ActiveDockTabItemChanged?.Invoke(this, eventArgs);
        }

        private void UpdateTitle()
        {
            DisplayName = string.IsNullOrEmpty(Subtitle)
                          ? ApplicationName
                          : Invariant($"{Subtitle} - {ApplicationName}");
        }

        protected virtual void OnWindowActivated(EventArgs eventArgs)
        {
            WindowActivated?.Invoke(this, eventArgs);
        }

        private void UpdateInputAndCommandBindings()
        {
            if (Window == null)
            {
                // Abort. This method is called again in EditorViewModel.OnActivated.
                return;
            }

            // Remove previous input bindings from main window and float windows.
            RemoveInputBindings(Window);
            RemoveInputBindings(DockControl);
            RemoveCommandBindings(DockControl);

            _inputBindings.Clear();
            _commandItemInputBindings.Clear();
            _commandBindings.Clear();

            // Get existing input bindings of main window.
            _inputBindings.AddRange(Window.InputBindings.OfType<InputBinding>());

            // Add input bindings for command items.
            //var commandItems = MenuManager.CommandItems
            //                              .Concat(ToolBarManager.CommandItems)
            //                              .OfType<CommandItem>()
            //                              .Distinct();
            //foreach (var commandItem in commandItems)
            //{
            //    if (commandItem?.Command != null
            //        && !(commandItem.Command is RoutedCommand)
            //        && commandItem.InputGestures != null)
            //    {
            //        foreach (var gesture in commandItem.InputGestures.OfType<InputGesture>())
            //        {
            //            var inputBinding = new InputBinding(commandItem.Command, gesture)
            //            {
            //                CommandParameter = commandItem.CommandParameter
            //            };

            //            _commandItemInputBindings.Add(inputBinding);
            //        }
            //    }
            //}

            // Get list of command bindings.
            // Command bindings could be defined in XAML or they are added to the window by 
            // extensions.
            _commandBindings.AddRange(Window.CommandBindings.OfType<CommandBinding>());

            // Add input bindings to main window and float windows.
            AddInputBindings(Window);
            AddInputBindings(DockControl);
            AddCommandBindings(DockControl);
        }


        private void AddInputBindings(Window window)
        {
            if (window == null)
                return;

            if (window != Window)
                window.InputBindings.AddRange(_inputBindings);

            window.InputBindings.AddRange(_commandItemInputBindings);
        }


        private void RemoveInputBindings(Window window)
        {
            if (window == null)
                return;

            if (window != Window)
                foreach (var inputBinding in _inputBindings)
                    window.InputBindings.Remove(inputBinding);

            foreach (var inputBinding in _commandItemInputBindings)
                window.InputBindings.Remove(inputBinding);
        }


        private void AddCommandBindings(Window window)
        {
            window?.CommandBindings.AddRange(_commandBindings);
        }


        private void RemoveCommandBindings(Window window)
        {
            if (window != null)
                foreach (var commandBinding in _commandBindings)
                    window.CommandBindings.Remove(commandBinding);
        }


        private void AddInputBindings(DockControl dockControl)
        {
            if (dockControl == null)
                return;

            foreach (var floatWindow in dockControl.FloatWindows)
                AddInputBindings(floatWindow);

            foreach (var autoHideOverlay in dockControl.AutoHideOverlays)
                AddInputBindings(autoHideOverlay);
        }


        private void RemoveInputBindings(DockControl dockControl)
        {
            if (dockControl == null)
                return;

            foreach (var floatWindow in dockControl.FloatWindows)
                RemoveInputBindings(floatWindow);

            foreach (var autoHideOverlay in dockControl.AutoHideOverlays)
                RemoveInputBindings(autoHideOverlay);
        }


        private void AddCommandBindings(DockControl dockControl)
        {
            if (dockControl == null)
                return;

            foreach (var floatWindow in dockControl.FloatWindows)
                AddCommandBindings(floatWindow);

            foreach (var autoHideOverlay in dockControl.AutoHideOverlays)
                AddCommandBindings(autoHideOverlay);
        }


        private void RemoveCommandBindings(DockControl dockControl)
        {
            if (dockControl == null)
                return;

            foreach (var floatWindow in dockControl.FloatWindows)
                RemoveCommandBindings(floatWindow);

            foreach (var autoHideOverlay in dockControl.AutoHideOverlays)
                RemoveCommandBindings(autoHideOverlay);
        }

        private void InitializeCommandItems()
        {
        }

        public void InvalidateUI()
        {
            //Logger.Debug("Invalidating UI.");

            // ----- Update menu
            //MenuManager.Update(MenuNodeCollections);

            // ----- Update toolbars
            // Backup previous toolbars to restore layout.
            //var previousToolBars = ToolBars?.ToArray();

            //// Create new toolbars.
            ////ToolBarManager.Update(ToolBarNodeCollections);

            //// Copy Band/BandIndex/Visibility from old view models to new view models.
            //if (previousToolBars != null)
            //{
            //    foreach (var toolBar in ToolBars)
            //    {
            //        var previousToolbar = previousToolBars.FirstOrDefault(tb => tb.CommandGroup.Name == toolBar.CommandGroup.Name);
            //        if (previousToolbar != null)
            //        {
            //            // Restore layout.
            //            toolBar.Band = previousToolbar.Band;
            //            toolBar.BandIndex = previousToolbar.BandIndex;
            //            toolBar.IsVisible = previousToolbar.IsVisible;
            //        }
            //    }
            //}

            //// ----- Update IDockTabItem context menu
            //_contextMenuManager.Update(DockContextMenuNodeCollections);

            // ----- Update input bindings
            UpdateInputAndCommandBindings();

            OnUIInvalidated(EventArgs.Empty);
        }

        protected virtual void OnUIInvalidated(EventArgs eventArgs)
        {
            UIInvalidated?.Invoke(this, eventArgs);
        }

        public void ActivateItem(object item)
        {
            if (item == null)
                throw new ArgumentNullException(nameof(item));

            var dockTabItem = item as IDockTabItem;
            if (dockTabItem == null)
                throw new ArgumentException("Item needs to implement IDockTabItem");

            DockStrategy.Show(dockTabItem);

#if DEBUG
            // Validation
            var activatable = dockTabItem as IActivatable;
            if (activatable != null)
            {
                Debug.Assert(activatable.IsOpen);
                Debug.Assert(activatable.IsActive);
                Debug.Assert(GetItems(true).Contains(item));
            }

            var screen = dockTabItem as IScreen;
            if (screen != null)
                Debug.Assert(screen.Conductor == this);
#endif
        }

        Task<bool> IScreenConductor.ActivateItemAsync(object item)
        {
            // Explicit interface implementation because this method is not really async.

            ActivateItem(item);
            return Task.FromResult(true);
        }

        public Task<bool> DeactivateItemAsync(object item, bool close)
        {
            var dockTabItem = item as IDockTabItem;
            if (dockTabItem == null
                || !close   // Temporary deactivation is not supported.
                || !DockStrategy.CanClose(dockTabItem))
            {
                return Task.FromResult(false);
            }

            DockStrategy.Close(dockTabItem);

#if DEBUG
            // Validation
            var activatable = dockTabItem as IActivatable;
            if (activatable != null)
            {
                Debug.Assert(!activatable.IsActive);
                Debug.Assert(!activatable.IsOpen);
                Debug.Assert(!GetItems(false).Contains(item));
            }

            var screen = dockTabItem as IScreen;
            if (screen != null)
                Debug.Assert(screen.Conductor == null);
#endif

            return Task.FromResult(true);
        }

        private IEnumerable<EditorDockTabItemViewModel> GetItems(bool onlyActive)
        {
            var items = this.GetDockElements()
                            .OfType<EditorDockTabItemViewModel>()
                            .Where(item => item.DockState != DockState.Hide);

            if (onlyActive)
                items = items.Where(item => item.IsActive);

            return items.Distinct();
        }
    }
}
