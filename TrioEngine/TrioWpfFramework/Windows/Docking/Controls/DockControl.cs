using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Interop;
using TrioWpfFramework.Mathematics;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.Interop;

namespace TrioWpfFramework.Windows.Docking.Controls
{
    [TemplatePart(Name = "PART_ContentPresenter", Type = typeof(ContentPresenter))]
    public class DockControl : ContentControl
    {

        private AutoHideBar _leftAutoHideBar;
        private AutoHideBar _rightAutoHideBar;
        private AutoHideBar _topAutoHideBar;
        private AutoHideBar _bottomAutoHideBar;
        private AutoHideBar[] _autoHideBars = Array.Empty<AutoHideBar>();
        private Panel _autoHidePanel;

        private ContentPresenter _contentPresenter;

        internal DockStrategy DockStrategy { get; private set; }

        internal DragManager DragManager { get; }

        internal FrameworkElement RootPane
        {
            get { return _contentPresenter.GetContentContainer<FrameworkElement>(); }
        }

        public ReadOnlyObservableCollection<DockTabPane> DockTabPanes { get; }
        private readonly ObservableCollection<DockTabPane> _dockTabPanes;

        public ReadOnlyObservableCollection<DockTabItem> DockTabItems { get; }
        private readonly ObservableCollection<DockTabItem> _dockTabItems;

        public ReadOnlyObservableCollection<FloatWindow> FloatWindows { get; }
        private readonly ObservableCollection<FloatWindow> _floatWindows;

        public ReadOnlyObservableCollection<AutoHideOverlay> AutoHideOverlays { get; }
        // AutoHideOverlays add/remove themselves to/from this list.
        internal readonly ObservableCollection<AutoHideOverlay> _autoHideOverlays;

        public static readonly DependencyProperty DockWidthProperty = DependencyProperty.RegisterAttached(
            "DockWidth",
            typeof(GridLength),
            typeof(DockControl),
            new FrameworkPropertyMetadata(
                DockHelper.BoxedGridLengthOneStar,
                FrameworkPropertyMetadataOptions.AffectsMeasure
                | FrameworkPropertyMetadataOptions.AffectsParentMeasure
                | FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                OnDockSizeChanged));
        

        public static readonly DependencyProperty DockHeightProperty = DependencyProperty.RegisterAttached(
            "DockHeight",
            typeof(GridLength),
            typeof(DockControl),
            new FrameworkPropertyMetadata(
                DockHelper.BoxedGridLengthOneStar,
                FrameworkPropertyMetadataOptions.AffectsMeasure
                | FrameworkPropertyMetadataOptions.AffectsParentMeasure
                | FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                OnDockSizeChanged));

        /// <summary>
        /// Initializes static members of the <see cref="DockControl"/> class.
        /// </summary>
        static DockControl()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(DockControl), new FrameworkPropertyMetadata(typeof(DockControl)));
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="DockControl"/> class.
        /// </summary>
        public DockControl()
        {
            // Create internal collections.
            _dockTabItems = new ObservableCollection<DockTabItem>();
            _dockTabPanes = new ObservableCollection<DockTabPane>();
            _floatWindows = new ObservableCollection<FloatWindow>();
            _autoHideOverlays = new ObservableCollection<AutoHideOverlay>();

            // Create read-only wrappers.
            DockTabItems = new ReadOnlyObservableCollection<DockTabItem>(_dockTabItems);
            DockTabPanes = new ReadOnlyObservableCollection<DockTabPane>(_dockTabPanes);
            FloatWindows = new ReadOnlyObservableCollection<FloatWindow>(_floatWindows);
            AutoHideOverlays = new ReadOnlyObservableCollection<AutoHideOverlay>(_autoHideOverlays);

            // Attach DragManager.
            DragManager = new DragManager(this);

            DataContextChanged += OnDataContextChanged;
            Loaded += OnLoaded;
            Unloaded += OnUnloaded;
        }

        public DockTabItem GetView(IDockTabItem viewModel)
        {
            for (int i = 0; i < _dockTabItems.Count; i++)
                if (_dockTabItems[i].DataContext == viewModel)
                    return _dockTabItems[i];

            return null;
        }

        private FloatWindow GetView(IFloatWindow viewModel)
        {
            for (int i = 0; i < _floatWindows.Count; i++)
                if (_floatWindows[i].DataContext == viewModel)
                    return _floatWindows[i];

            return null;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs eventArgs)
        {
            if (!IsLoaded)
                return; // Wait until control is loaded.

            var oldContext = eventArgs.OldValue as IDockControl;
            if (oldContext != null)
                UnregisterEventHandlers(oldContext);

            var newContext = eventArgs.NewValue as IDockControl;
            if (newContext != null)
                RegisterEventHandlers(newContext);

            CommandManager.InvalidateRequerySuggested();
        }

        private void RegisterEventHandlers(IDockControl dataContext)
        {
            if (dataContext == null)
                return;

            // Attach to IDockControl.
            if (dataContext.DockStrategy == null)
                throw new DockException("IDockControl.DockStrategy must not be null.");

            DockStrategy = dataContext.DockStrategy;

            // Observe IDockControl properties.
            PropertyChangedEventManager.AddHandler(dataContext, OnDockStrategyChanged, nameof(IDockControl.DockStrategy));
            PropertyChangedEventManager.AddHandler(dataContext, OnActiveItemChanged, nameof(IDockControl.ActiveDockTabItem));
            //PropertyChangedEventManager.AddHandler(dataContext, OnActivePaneChanged, nameof(IDockControl.ActiveDockTabPane));

            // The ICollectionView is used to filter IFloatWindows.
            var collectionView = CollectionViewSource.GetDefaultView(dataContext.FloatWindows);
            var collectionViewLiveShaping = collectionView as ICollectionViewLiveShaping;
            if (collectionViewLiveShaping != null && collectionViewLiveShaping.CanChangeLiveFiltering)
            {
                collectionViewLiveShaping.LiveFilteringProperties.Clear();
                collectionViewLiveShaping.LiveFilteringProperties.Add(nameof(IFloatWindow.IsVisible));
                collectionViewLiveShaping.IsLiveFiltering = true;
                collectionView.Filter = floatWindow => ((IFloatWindow)floatWindow).IsVisible;
            }
            CollectionChangedEventManager.AddHandler(collectionView, OnFloatWindowsChanged);
        }


        private void UnregisterEventHandlers(IDockControl dataContext)
        {
            if (dataContext == null)
                return;

            // Detach from IDockControl.
            DockStrategy = null;
            PropertyChangedEventManager.RemoveHandler(dataContext, OnDockStrategyChanged, nameof(IDockControl.DockStrategy));
            PropertyChangedEventManager.RemoveHandler(dataContext, OnActiveItemChanged, nameof(IDockControl.ActiveDockTabItem));
            //PropertyChangedEventManager.RemoveHandler(dataContext, OnActivePaneChanged, nameof(IDockControl.ActiveDockTabPane));

            var collectionView = CollectionViewSource.GetDefaultView(dataContext.FloatWindows);
            CollectionChangedEventManager.RemoveHandler(collectionView, OnFloatWindowsChanged);
        }

        private void OnDockStrategyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            DockStrategy = this.GetViewModel()?.DockStrategy;

            // Update menu items and buttons.
            CommandManager.InvalidateRequerySuggested();
        }

        private void OnActiveItemChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            Debug.Assert(DockStrategy != null);
            Debug.Assert(DockStrategy.DockControl.ActiveDockTabItem == null || DockStrategy.DockControl.ActiveDockTabPane != null,
                         "IDockControl.ActiveDockTabPane needs to be set before IDockControl.ActiveDockTabItem.");
            Debug.Assert(DockStrategy.DockControl.ActiveDockTabPane == null || DockStrategy.DockControl.ActiveDockTabPane.SelectedItem == DockStrategy.DockControl.ActiveDockTabItem,
                         "IDockControl.ActiveDockTabPane.SelectedItem needs to match IDockControl.ActiveDockTabItem.");

            var dockTabPaneVM = DockStrategy.DockControl.ActiveDockTabPane;
            var dockTabItemVM = DockStrategy.DockControl.ActiveDockTabItem;
            if (dockTabPaneVM == null || dockTabItemVM == null)
            {
                CloseAutoHidePanes();
                return;
            }

            if (dockTabItemVM.DockState == DockState.Dock)
            {
                CloseAutoHidePanes();
            }
            else if (dockTabItemVM.DockState == DockState.Float)
            {
                CloseAutoHidePanes();
                var floatWindowVM = DockStrategy?.GetFloatWindow(dockTabItemVM);
                GetView(floatWindowVM)?.Activate();
            }
            else if (dockTabItemVM.DockState == DockState.AutoHide)
            {
                ShowAutoHidePane(dockTabPaneVM, dockTabItemVM);
            }

            GetView(dockTabItemVM)?.Activate();
        }


        /// <summary>
        /// Registers the specified <see cref="DockTabPane"/>.
        /// </summary>
        /// <param name="dockTabPane">The <see cref="DockTabPane"/> to add.</param>
        internal void Register(DockTabPane dockTabPane)
        {
            Debug.Assert(dockTabPane != null);

            if (!_dockTabPanes.Contains(dockTabPane))
                _dockTabPanes.Add(dockTabPane);
        }


        /// <summary>
        /// Unregisters the specified <see cref="DockTabPane"/>.
        /// </summary>
        /// <param name="dockTabPane">The <see cref="DockTabPane"/> to remove.</param>
        internal void Unregister(DockTabPane dockTabPane)
        {
            _dockTabPanes.Remove(dockTabPane);
        }


        /// <summary>
        /// Registers the specified <see cref="DockTabItem"/>.
        /// </summary>
        /// <param name="dockTabItem">The <see cref="DockTabItem"/> to add.</param>
        internal void Register(DockTabItem dockTabItem)
        {
            Debug.Assert(dockTabItem != null);

            if (!_dockTabItems.Contains(dockTabItem))
                _dockTabItems.Add(dockTabItem);

            // Activate the DockTabItem if it is the IDockControl.ActiveDockTabItem.
            if (DockStrategy != null)
            {
                var dockTabItemVM = dockTabItem.GetViewModel();
                if (dockTabItemVM != null && DockStrategy.DockControl.ActiveDockTabItem == dockTabItemVM)
                    dockTabItem.Activate();
            }
        }


        /// <summary>
        /// Unregisters the specified <see cref="DockTabItem"/>.
        /// </summary>
        /// <param name="dockTabItem">The <see cref="DockTabItem"/> to remove.</param>
        internal void Unregister(DockTabItem dockTabItem)
        {
            Debug.Assert(dockTabItem != null);

            _dockTabItems.Remove(dockTabItem);
        }


        private void OnLoaded(object sender, RoutedEventArgs eventArgs)
        {
            var viewModel = DataContext as IDockControl;

            if (viewModel != null)
                viewModel.DockControl = this;

            RegisterEventHandlers(viewModel);
            LoadFloatWindows();
        }


        private void OnUnloaded(object sender, RoutedEventArgs eventArgs)
        {
            CloseAutoHidePanes();
            UnloadFloatWindows();

            var viewModel = DataContext as IDockControl;
            UnregisterEventHandlers(viewModel);

            if (viewModel != null)
                viewModel.DockControl = null;
        }

        public static GridLength GetDockWidth(DependencyObject obj)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            return (GridLength)obj.GetValue(DockWidthProperty);
        }

        public static void SetDockWidth(DependencyObject obj, GridLength value)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            obj.SetValue(DockWidthProperty, value);
        }

        public static GridLength GetDockHeight(DependencyObject obj)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            return (GridLength)obj.GetValue(DockHeightProperty);
        }

        public static void SetDockHeight(DependencyObject obj, GridLength value)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            obj.SetValue(DockHeightProperty, value);
        }

        public override void OnApplyTemplate()
        {
            _contentPresenter = null;

            foreach (var autoHideBar in _autoHideBars)
                autoHideBar.TargetArea = null;

            _leftAutoHideBar = null;
            _rightAutoHideBar = null;
            _topAutoHideBar = null;
            _bottomAutoHideBar = null;
            _autoHideBars = null;

            if (_autoHidePanel != null)
            {
                _autoHidePanel.Children.Clear();
                _autoHidePanel = null;
            }

            base.OnApplyTemplate();

            _contentPresenter = GetTemplateChild("PART_ContentPresenter") as ContentPresenter;
            _autoHidePanel = GetTemplateChild("PART_AutoHidePanel") as Panel;
            _leftAutoHideBar = GetTemplateChild("PART_AutoHideBarLeft") as AutoHideBar;
            _rightAutoHideBar = GetTemplateChild("PART_AutoHideBarRight") as AutoHideBar;
            _topAutoHideBar = GetTemplateChild("PART_AutoHideBarTop") as AutoHideBar;
            _bottomAutoHideBar = GetTemplateChild("PART_AutoHideBarBottom") as AutoHideBar;

            var autoHideBars = new List<AutoHideBar>(4);
            if (_leftAutoHideBar != null)
            {
                _leftAutoHideBar.SetBinding(ItemsControl.ItemsSourceProperty, new Binding(nameof(IDockControl.AutoHideLeft)));
                autoHideBars.Add(_leftAutoHideBar);
            }
            if (_rightAutoHideBar != null)
            {
                _rightAutoHideBar.SetBinding(ItemsControl.ItemsSourceProperty, new Binding(nameof(IDockControl.AutoHideRight)));
                autoHideBars.Add(_rightAutoHideBar);
            }
            if (_topAutoHideBar != null)
            {
                _topAutoHideBar.SetBinding(ItemsControl.ItemsSourceProperty, new Binding(nameof(IDockControl.AutoHideTop)));
                autoHideBars.Add(_topAutoHideBar);
            }
            if (_bottomAutoHideBar != null)
            {
                _bottomAutoHideBar.SetBinding(ItemsControl.ItemsSourceProperty, new Binding(nameof(IDockControl.AutoHideBottom)));
                autoHideBars.Add(_bottomAutoHideBar);
            }

            _autoHideBars = autoHideBars.ToArray();

            foreach (var autoHideBar in _autoHideBars)
                autoHideBar.TargetArea = _autoHidePanel;
        }


        private void ShowAutoHidePane(IDockTabPane dockTabPane, IDockTabItem dockTabItem)
        {
            CloseAutoHidePanesExcept(dockTabPane);
            foreach (var autoHideBar in _autoHideBars)
            {
                var autoHidePane = autoHideBar.ShowAutoHidePane(dockTabPane, dockTabItem, true);
                if (autoHidePane != null)
                    break;
            }
        }

        internal void CloseAutoHidePanes()
        {
            foreach (var autoHideBar in _autoHideBars)
                autoHideBar.CloseAutoHidePanes();
        }

        internal void CloseAutoHidePanesExcept(IDockTabPane dockTabPane)
        {
            foreach (var autoHideBar in _autoHideBars)
                autoHideBar.CloseAutoHidePanesExcept(dockTabPane);
        }
        private static void OnDockSizeChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            // Propagate the DockWidth/Height to children.
            if (dependencyObject is DockAnchorPane)
            {
                var dockAnchorPane = (DockAnchorPane)dependencyObject;
                var child = dockAnchorPane.ChildPane;
                if (child != null)
                {
                    Debug.Assert(child is DockAnchorPane || child is DockSplitPane || child is DockTabPane);

                    // If this pane is a *-size pane we do not change children that have absolute size.
                    var oldLength = (GridLength)child.GetValue(eventArgs.Property);
                    var newLength = (GridLength)eventArgs.NewValue;
                    if ((oldLength.IsStar && newLength.IsStar) || (!oldLength.IsStar && !newLength.IsStar))
                        child.SetValue(eventArgs.Property, eventArgs.NewValue);
                }
            }
            else if (dependencyObject is DockSplitPane)
            {
                var dockSplitPane = (DockSplitPane)dependencyObject;
                var orientation = dockSplitPane.Orientation;
                for (int i = 0; i < dockSplitPane.Items.Count; i++)
                {
                    var child = dockSplitPane.ItemContainerGenerator.ContainerFromIndex(i);
                    if (child != null)
                    {
                        Debug.Assert(child is DockAnchorPane || child is DockSplitPane || child is DockTabPane);

                        if (orientation == Orientation.Vertical && eventArgs.Property == DockWidthProperty
                            || orientation == Orientation.Horizontal && eventArgs.Property == DockHeightProperty)
                        {
                            child.SetValue(eventArgs.Property, eventArgs.NewValue);
                        }
                    }
                }
            }
            else if (dependencyObject is DockTabPane)
            {
                var dockTabPane = (DockTabPane)dependencyObject;
                for (int i = 0; i < dockTabPane.Items.Count; i++)
                {
                    var dockTabItem = dockTabPane.ItemContainerGenerator.ContainerFromIndex(i) as DockTabItem;
                    if (dockTabItem != null)
                    {
                        // If this pane is a *-size pane we do not change children that have absolute size.
                        var oldLength = (GridLength)dockTabItem.GetValue(eventArgs.Property);
                        var newLength = (GridLength)eventArgs.NewValue;
                        if ((oldLength.IsStar && newLength.IsStar) || (!oldLength.IsStar && !newLength.IsStar))
                            dockTabItem.SetValue(eventArgs.Property, eventArgs.NewValue);
                    }
                }
            }
            else if (dependencyObject is DockTabItem)
            {
                var dockTabItem = (DockTabItem)dependencyObject;
                var dockTabPane = ItemsControl.ItemsControlFromItemContainer(dockTabItem) as DockTabPane;
                if (dockTabPane != null && dockTabPane.Items.Count == 1)
                {
                    // Copy the relevant property to the DockTabPane if this is the only child.
                    dockTabPane.SetValue(eventArgs.Property, eventArgs.NewValue);
                }
            }
        }

        private void OnFloatWindowsChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            UpdateFloatWindows();
        }


        internal void UpdateFloatWindows()
        {
            if (DockStrategy == null)
                return;

            var owner = Window.GetWindow(this);

            // Close obsolete FloatWindows.
            for (int i = _floatWindows.Count - 1; i >= 0; i--)
            {
                var floatWindow = _floatWindows[i];
                var floatWindowVM = floatWindow.GetViewModel();
                if (floatWindowVM == null
                    || !DockStrategy.DockControl.FloatWindows.Contains(floatWindowVM)
                    || !DockStrategy.IsVisible(floatWindowVM))
                {
                    // ----- Close FloatWindow.
                    floatWindow.Close();
                    _floatWindows.RemoveAt(i);
                }
            }

            // Open new FloatWindows.
            for (int i = 0; i < DockStrategy.DockControl.FloatWindows.Count; i++)
            {
                var floatWindowVM = DockStrategy.DockControl.FloatWindows[i];
                if (DockStrategy.IsVisible(floatWindowVM) && GetView(floatWindowVM) == null)
                {
                    // ----- Open FloatWindow.

                    // Make sure that the floating window stays on the screen.
                    // At least 30 pixels must be visible.
                    const double safety = 30;
                    double screenLeft = SystemParameters.VirtualScreenLeft;
                    double screenTop = SystemParameters.VirtualScreenTop;
                    double screenRight = screenLeft + SystemParameters.VirtualScreenWidth;
                    double screenBottom = screenTop + SystemParameters.VirtualScreenHeight;
                    floatWindowVM.Left = Math.Min(Math.Max(floatWindowVM.Left, screenLeft), screenRight - safety);
                    floatWindowVM.Top = Math.Min(Math.Max(floatWindowVM.Top, screenTop), screenBottom - safety);

                    var floatWindow = new FloatWindow(this)
                    {
                        DataContext = floatWindowVM,
                        Owner = owner,
                    };

                    bool autoWidth = Numeric.IsNaN(floatWindowVM.Width);
                    bool autoHeight = Numeric.IsNaN(floatWindowVM.Height);
                    if (autoWidth && autoHeight)
                        floatWindow.SizeToContent = SizeToContent.WidthAndHeight;
                    else if (autoWidth)
                        floatWindow.SizeToContent = SizeToContent.Width;
                    else if (autoHeight)
                        floatWindow.SizeToContent = SizeToContent.Height;
                    else
                        floatWindow.SizeToContent = SizeToContent.Manual;

                    floatWindow.SetBinding(ContentProperty, new Binding(nameof(IFloatWindow.RootPane)));
                    floatWindow.SetBinding(WidthProperty, new Binding(nameof(IFloatWindow.Width)) { Mode = BindingMode.TwoWay });
                    floatWindow.SetBinding(HeightProperty, new Binding(nameof(IFloatWindow.Height)) { Mode = BindingMode.TwoWay });
                    floatWindow.SetBinding(Window.LeftProperty, new Binding(nameof(IFloatWindow.Left)) { Mode = BindingMode.TwoWay });
                    floatWindow.SetBinding(Window.TopProperty, new Binding(nameof(IFloatWindow.Top)) { Mode = BindingMode.TwoWay });

                    floatWindow.Show();

                    // Bind WindowState after showing the window. Otherwise, it could be maximized
                    // on the wrong screen.
                    floatWindow.SetBinding(Window.WindowStateProperty, new Binding(nameof(IFloatWindow.WindowState)) { Mode = BindingMode.TwoWay });

                    _floatWindows.Add(floatWindow);
                }
            }
        }


        private void LoadFloatWindows()
        {
            // Show temporarily hidden FloatWindows.
            for (int i = 0; i < _floatWindows.Count; i++)
                _floatWindows[i].Show();

            // Show new FloatWindows.
            UpdateFloatWindows();

            // We need to activate the main window. If it is not activated and a FloatWindow
            // was opened in UpdateDockState() and after the start Focus() is called on the 
            // FloatWindow Content then the main window will not be able to get the focus back. 
            //--> WPF Bug?
            var window = Window.GetWindow(this);
            if (window != null)
            {
                window.Activate();
                window.StateChanged += OnWindowStateChanged;
            }
        }


        private void UnloadFloatWindows()
        {
            // When the DockControl is unloaded (e.g. because it is in a tab control), the
            // DragManager does not work.
            // --> Hide all floating windows together with the DockControl.
            for (int i = _floatWindows.Count - 1; i >= 0; i--)
            {
                var floatWindow = _floatWindows[i];
                floatWindow.Hide();
            }

            var window = Window.GetWindow(this);
            if (window != null)
                window.StateChanged -= OnWindowStateChanged;
        }


        private void OnWindowStateChanged(object sender, EventArgs eventArgs)
        {
            // WPF does not restore child windows correctly when the main window is minimized and
            // restored. This needs to be handled explicitly.

            // Note: Do not use SystemCommands.MinimizeWindow(floatWindow) and
            // SystemCommands.RestoreWindow(floatWindow)! These methods work correctly when
            // used the first time, but screw up the window state when used multiple times.
            var window = (Window)sender;
            if (window.WindowState == WindowState.Minimized)
            {
                for (int i = 0; i < _floatWindows.Count; i++)
                {
                    var floatWindow = _floatWindows[i];
                    Win32.ShowWindow(new WindowInteropHelper(floatWindow).Handle, ShowWindowStyles.SW_MINIMIZE);
                }
            }
            else
            {
                for (int i = 0; i < _floatWindows.Count; i++)
                {
                    var floatWindow = _floatWindows[i];
                    if (floatWindow.WindowState == WindowState.Minimized)
                        Win32.ShowWindow(new WindowInteropHelper(floatWindow).Handle, ShowWindowStyles.SW_RESTORE);
                }
            }
        }
    }
}
