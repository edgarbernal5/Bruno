using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using TrioWpfFramework.Mathematics;
using TrioWpfFramework.Windows.Docking.Controls;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.Interop;

namespace TrioWpfFramework.Windows.Docking
{
    internal class DragManager
    {
        // Tolerance when dragging items in DockTabPanel.
        private const double VerticalTolerance = 24;

        private readonly DockControl _dockControl;
        private DockStrategy _dockStrategy;

        // The items that are dragged and the currently selected item.
        private readonly List<IDockTabItem> _draggedItems = new List<IDockTabItem>();
        private IDockTabItem _activeItem;

        // The FloatWindow that is currently being dragged.
        private FloatWindow _floatWindow;

        // The DockTabPane in which the items are currently being dragged.
        private DockTabPane _targetDockTabPane;

        // Flag to see if the docking layout has changed.
        private bool _layoutChanged;

        // The initial mouse position.
        private Point _initialMousePosition;
        private bool _dragDeltaExceeded;

        // The offset between the mouse cursor and the origin of the dragged element.
        private Vector _mouseOffset;

        // The initial size of the dragged elements.
        private Size _initialSize;

        // Is Float allowed as the final dock state?
        private bool _canFloat;

        // Rollback information.
        private DockState _originalDockState;
        private IFloatWindow _originalFloatWindow;
        private double _originalFloatLeft;
        private double _originalFloatTop;


        internal bool IsDragging
        {
            get { return _draggedItems.Count > 0; }
        }


        private bool IsDraggingFloatWindow
        {
            get { return _floatWindow != null; }
        }

        private bool IsDraggingDockTabItems
        {
            get { return IsDragging && !IsDraggingFloatWindow; }
        }


        /// <summary>
        /// Initializes a new instance of the <see cref="DragManager"/> class.
        /// </summary>
        /// <param name="dockControl">The <see cref="DockControl"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="dockControl"/> is <see langword="null"/>.
        /// </exception>
        internal DragManager(DockControl dockControl)
        {
            if (dockControl == null)
                throw new ArgumentNullException(nameof(dockControl));

            _dockControl = dockControl;

            Reset();

            // Register event handlers.
            ((INotifyCollectionChanged)_dockControl.DockTabPanes).CollectionChanged += OnDockTabPanesChanged;
        }

        /// <summary>
        /// Resets all fields.
        /// </summary>
        private void Reset()
        {
            Debug.Assert(Mouse.Captured == null);
            Debug.Assert(_borderDockIndicators == null);
            Debug.Assert(_paneDockIndicators == null);
            Debug.Assert(
                _dockStrategy == null || !_dockStrategy.DockControl.GetDockElements().OfType<DockTabItemProxy>().Any(),
                "All item proxies should have been removed.");

            _dockStrategy = null;
            _draggedItems.Clear();
            _activeItem = null;
            _floatWindow = null;
            _targetDockTabPane = null;
            _layoutChanged = false;
            _initialMousePosition = new Point(double.NaN, double.NaN);
            _dragDeltaExceeded = false;
            _mouseOffset = new Vector(double.NaN, double.NaN);
            _initialSize = new Size(double.NaN, double.NaN);
            _canFloat = true;
            _originalDockState = DockState.Hide;
            _originalFloatWindow = null;
            _originalFloatLeft = 0;
            _originalFloatTop = 0;
        }


        /// <summary>
        /// Starts a drag operation.
        /// </summary>
        /// <param name="floatWindow">The <see cref="FloatWindow"/> to be dragged.</param>
        /// <param name="dockTabPane">The <see cref="DockTabPane"/> to be dragged.</param>
        /// <param name="dockTabItem">The <see cref="DockTabItem"/> to be dragged.</param>
        /// <returns>
        /// <see langword="true" /> if the drag operation has been started; otherwise,
        /// <see langword="false" /> if the drag operation could not be started (e.g. because the
        /// mouse could not be captured).
        /// </returns>
        private bool BeginDrag(FloatWindow floatWindow, DockTabPane dockTabPane, DockTabItem dockTabItem)
        {
            _dockStrategy = _dockControl.GetViewModel()?.DockStrategy;
            if (_dockStrategy == null || _dockStrategy.DockControl.IsLocked)
            {
                Reset();
                return false;
            }

            FrameworkElement element = null;
            IDockTabPane draggedPane = null;
            IDockTabItem draggedItem = null;
            if (floatWindow != null)
            {
                // User is dragging a FloatWindow.
                // (Note: Dragging of FloatWindows with nested layouts is not supported.)
                draggedPane = floatWindow.GetViewModel()?.RootPane as IDockTabPane;
                element = floatWindow;
                _floatWindow = floatWindow;
                _initialSize = floatWindow.RenderSize;

                // Start dragging immediately.
                _dragDeltaExceeded = true;
            }
            else if (dockTabItem != null)
            {
                // User is dragging a DockTabItem in a DockTabPanel.
                draggedItem = dockTabItem.GetViewModel();
                element = dockTabItem;
                _targetDockTabPane = dockTabPane;
                _initialSize = dockTabPane.RenderSize;

                // Start dragging when threshold is exceeded.
                _initialMousePosition = WpfWindowsExtensions.GetMousePosition(_dockControl);
                _dragDeltaExceeded = false;
            }
            else if (dockTabPane != null)
            {
                // User is dragging a DockTabPane.
                draggedPane = dockTabPane.GetViewModel();
                element = dockTabPane;
                _initialSize = dockTabPane.RenderSize;
                _initialMousePosition = WpfWindowsExtensions.GetMousePosition(_dockControl);

                // Start dragging when threshold is exceeded.
                _initialMousePosition = WpfWindowsExtensions.GetMousePosition(_dockControl);
                _dragDeltaExceeded = false;
            }

            if (draggedPane == null && draggedItem == null)
            {
                Reset();
                return false;
            }

            // When the user is dragging the FloatWindow, the mouse is captured by Win32 move window
            // loop. When dragging a DockTabPane or DockTabItem, the mouse needs to be
            // captured to receive mouse events.
            if (_floatWindow == null)
            {
                if (!_dockControl.CaptureMouse())
                {
                    // Failed to capture the mouse.
                    Reset();
                    return false;
                }

                _dockControl.LostMouseCapture += OnLostMouseCapture;
                _dockControl.MouseLeftButtonUp += OnMouseLeftButtonUp;
                _dockControl.MouseMove += OnMouseMove;
                _dockControl.PreviewKeyDown += OnPreviewKeyDown;
                if (_targetDockTabPane != null)
                    _targetDockTabPane.PreviewKeyDown += OnPreviewKeyDown;
            }

            _dockStrategy.Begin();

            if (draggedPane != null)
            {
                _dockStrategy.Activate(draggedPane);
                _activeItem = draggedPane.SelectedItem;
                foreach (var item in draggedPane.Items)
                    if (item.DockState == draggedPane.DockState)
                        _draggedItems.Add(item);
            }
            else
            {
                Debug.Assert(draggedItem != null);

                _dockStrategy.Activate(draggedItem);
                _activeItem = draggedItem;
                _draggedItems.Add(draggedItem);
            }

            Debug.Assert(_draggedItems.Count > 0);

            // Determine whether dragged items may end in a FloatWindow.
            _canFloat = CanFloat();

            // Store the mouse offset relative to the dragged element.
            _mouseOffset = (Vector)WpfWindowsExtensions.GetMousePosition(element);

            // Remember information needed for a rollback.
            ReplaceItemsWithProxies(draggedPane ?? _targetDockTabPane.GetViewModel());
            _originalDockState = _draggedItems[0].DockState;
            BackupFloatWindowPosition();

            // Override mouse cursors. (Mouse cursor should not change to caret over text editor.)
            Mouse.OverrideCursor = Cursors.Arrow;

            return true;
        }


        /// <summary>
        /// Handles mouse move events when the drag operation is in progress.
        /// </summary>
        private void Drag()
        {
            Debug.Assert(IsDragging);

            if (!_dragDeltaExceeded)
            {
                Point mousePosition = WpfWindowsExtensions.GetMousePosition(_dockControl);
                Vector dragDelta = mousePosition - _initialMousePosition;
                if (Math.Abs(dragDelta.X) > SystemParameters.MinimumHorizontalDragDistance
                    || Math.Abs(dragDelta.Y) > SystemParameters.MinimumVerticalDragDistance)
                {
                    _dragDeltaExceeded = true;
                }
                else
                {
                    // Do nothing.
                    return;
                }
            }

            if (_targetDockTabPane != null && IsMouseOver(_targetDockTabPane.GetItemsPanel(), VerticalTolerance))
            {
                // Drag items inside a DockTabPanel.
                DragDockTabItems();
                return;
            }

            if (_floatWindow == null)
            {
                // Move the items into a FloatWindow.
                DragDockTabItemsIntoFloatWindow();
            }

            // Move the FloatWindow to the mouse position.
            DragFloatWindow();

            // Show DockIndicators and check possible dock positions.
            UpdateDockIndicators();

            if (!HasResult(_paneDockIndicators) && !HasResult(_borderDockIndicators))
            {
                // Check whether mouse cursor is over other DockTabItem tab and
                // move items into DockTabPane.
                DragFloatWindowIntoDockTabPanel();
            }
        }


        /// <summary>
        /// Drags the items to the mouse position within the <see cref="DockTabPanel"/>.
        /// </summary>
        private void DragDockTabItems()
        {
            Debug.Assert(_targetDockTabPane != null);

            // Switch item positions in DockTabPane.
            var hitItemVM = GetDockTabItemAtMouse(_targetDockTabPane, VerticalTolerance)?.GetViewModel();
            if (hitItemVM != null && !_draggedItems.Contains(hitItemVM))
            {
                var paneVM = _targetDockTabPane.GetViewModel();
                int index = paneVM.Items.IndexOf(hitItemVM);
                foreach (var item in _draggedItems)
                {
                    int oldIndex = paneVM.Items.IndexOf(item);
                    paneVM.Items.Move(oldIndex, index);

                    if (index < oldIndex)
                        index++;
                }

                _layoutChanged = true;
                _dockStrategy.Cleanup();
            }

            // Ensure that the visual tree is up-to-date.
            _targetDockTabPane.GetItemsPanel().UpdateLayout();

            // Apply a horizontal offset to move the dragged items with the mouse.
            SetTranslateTransform();
        }


        /// <summary>
        /// Called to move the <see cref="FloatWindow"/>.
        /// </summary>
        private void DragFloatWindow()
        {
            Debug.Assert(_floatWindow != null);

            // Do nothing. FloatWindow is moved in Win32 move window loop.
            _layoutChanged = true;
        }


        /// <summary>
        /// Move the dragged items from the current <see cref="DockTabPanel"/> into a
        /// <see cref="FloatWindow"/>.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Maintainability", "CA1506:AvoidExcessiveClassCoupling")]
        private void DragDockTabItemsIntoFloatWindow()
        {
            Debug.Assert(_floatWindow == null);

            if (_targetDockTabPane != null)
            {
                ClearTranslateTransform();

                _targetDockTabPane.PreviewKeyDown -= OnPreviewKeyDown;
                _targetDockTabPane = null;
            }

            _dockControl.LostMouseCapture -= OnLostMouseCapture;
            _dockControl.MouseLeftButtonUp -= OnMouseLeftButtonUp;
            _dockControl.MouseMove -= OnMouseMove;
            _dockControl.PreviewKeyDown -= OnPreviewKeyDown;

            _dockControl.ReleaseMouseCapture();

            // Remove the dragged items from their current location.
            foreach (var item in _draggedItems)
            {
                DockHelper.Remove(_dockStrategy.DockControl.RootPane, item);

                foreach (var floatWindow in _dockStrategy.DockControl.FloatWindows)
                    DockHelper.Remove(floatWindow, item);

                DockHelper.Remove(_dockStrategy.DockControl.AutoHideLeft, item);
                DockHelper.Remove(_dockStrategy.DockControl.AutoHideRight, item);
                DockHelper.Remove(_dockStrategy.DockControl.AutoHideTop, item);
                DockHelper.Remove(_dockStrategy.DockControl.AutoHideBottom, item);
            }

            // Move items into a new FloatWindow.
            foreach (var item in _draggedItems)
                item.DockState = DockState.Float;

            var newPaneVM = _dockStrategy.CreateDockTabPane(_draggedItems[0], DockState.Float);
            for (int i = 1; i < _draggedItems.Count; i++)
                newPaneVM.Items.Add(_draggedItems[i]);

            var floatWindowVM = _dockStrategy.CreateFloatWindow();
            floatWindowVM.RootPane = newPaneVM;
            _dockStrategy.DockControl.FloatWindows.Add(floatWindowVM);
            _dockStrategy.Activate(_activeItem);
            _dockStrategy.Cleanup();

            // Get the newly created FloatWindow (view) from the DockControl.
            _floatWindow = GetFloatWindow(floatWindowVM);

            Debug.Assert(_floatWindow != null);

            LimitFloatWindowSize(_floatWindow, _initialSize);

            // Limit mouse offset to FloatWindow size.
            double actualWidth = _floatWindow.ActualWidth;
            if (actualWidth > 0)
                _mouseOffset.X = Math.Min(_mouseOffset.X, actualWidth / 2);

            Point position = GetFloatWindowPosition();
            _floatWindow.Left = position.X;
            _floatWindow.Top = position.Y;

            // Wait until FloatWindow is loaded, initiate the Win32 move window loop.
            _floatWindow.Dispatcher.BeginInvoke(new Action(() =>
            {
                if (_floatWindow != null)
                {
                    LimitFloatWindowSize(_floatWindow, _initialSize);

                    // Limit mouse offset to FloatWindow size.
                    _mouseOffset.X = Math.Min(_mouseOffset.X, _floatWindow.ActualWidth / 2);
                    _mouseOffset.Y = Math.Max(_mouseOffset.Y, 8);

                    Point pos = GetFloatWindowPosition();
                    _floatWindow.Left = pos.X;
                    _floatWindow.Top = pos.Y;

                    if (Mouse.LeftButton == MouseButtonState.Pressed)
                        _floatWindow.DragMove();
                    else
                        EndDrag(true);
                }
            }), DispatcherPriority.Loaded); // Important: Action needs to be invoked before input.
        }


        /// <summary>
        /// Tries to move the dragged items from the <see cref="FloatWindow"/> into a
        /// <see cref="DockTabPanel"/>.
        /// </summary>
        private void DragFloatWindowIntoDockTabPanel()
        {
            Debug.Assert(_targetDockTabPane == null);
            Debug.Assert(_floatWindow != null);

            var dockTabPane = GetTargetPane() as DockTabPane;
            if (dockTabPane == null)
                return; // No DockTabPane (view).

            var dockTabPaneVM = dockTabPane.GetViewModel();
            if (dockTabPaneVM == null)
                return; // No IDockTabPane (view-model).

            if (GetDockTabItemAtMouse(dockTabPane) == null)
                return; // No DockTabItem hit.

            if (!CanDock(dockTabPaneVM, DockPosition.Inside))
                return; // Docking not allowed.

            // Remove currently dragged FloatWindow.
            var floatWindowVM = _floatWindow.GetViewModel();
            foreach (var item in _draggedItems)
                DockHelper.Remove(floatWindowVM, item);

            _floatWindow = null;
            Win32.ReleaseCapture(); // Exit Win32 move window loop.

            // Add items into target DockTabPane.
            _targetDockTabPane = dockTabPane;
            foreach (var item in _draggedItems)
            {
                item.DockState = dockTabPaneVM.DockState;
                dockTabPaneVM.Items.Add(item);
            }

            // Make sure the current item is selected in DockTabPane.
            _dockStrategy.Activate(_activeItem);

            // When the Win32 move window loop exits, the DockControl receives a LostMouseCapture
            // event. --> Defer dragging of the DockTabItems.
            _dockControl.Dispatcher.BeginInvoke(new Action(() =>
            {
                if (!_dockControl.IsMouseCaptured)
                {
                    if (!_dockControl.CaptureMouse())
                    {
                        // Failed to capture the mouse.
                        EndDrag(false);
                        return;
                    }
                    _dockControl.LostMouseCapture += OnLostMouseCapture;
                    _dockControl.MouseMove += OnMouseMove;
                    _dockControl.MouseLeftButtonUp += OnMouseLeftButtonUp;
                    _dockControl.PreviewKeyDown += OnPreviewKeyDown;

                    _targetDockTabPane.PreviewKeyDown += OnPreviewKeyDown;

                    DragDockTabItems();
                }
            }));

            HideBorderDockIndicators();
            HidePaneIndicators();

            _dockControl.UpdateFloatWindows();

            _layoutChanged = true;
        }


        /// <summary>
        /// Ends the drag operation.
        /// </summary>
        /// <param name="commit">
        /// <see langword="true"/> to commit the changes; otherwise, <see langword="false"/> to
        /// revert the changes.
        /// </param>
        internal void EndDrag(bool commit)
        {
            Debug.Assert(IsDragging);

            if (!_dragDeltaExceeded)
                commit = false;

            if (_targetDockTabPane != null)
            {
                _targetDockTabPane.PreviewKeyDown -= OnPreviewKeyDown;

                if (_dragDeltaExceeded)
                    ClearTranslateTransform();
            }

            if (IsDraggingDockTabItems)
            {
                // Remove event handlers.
                _dockControl.LostMouseCapture -= OnLostMouseCapture;
                _dockControl.MouseLeftButtonUp -= OnMouseLeftButtonUp;
                _dockControl.MouseMove -= OnMouseMove;
                _dockControl.PreviewKeyDown -= OnPreviewKeyDown;

                _dockControl.ReleaseMouseCapture();
            }

            if (commit)
            {
                Commit();
            }
            else
            {
                Win32.ReleaseCapture(); // Exit Win32 move window loop.
                Rollback();
            }

            // Get rid of DockIndicators.
            HideBorderDockIndicators();
            HidePaneIndicators();

            // Finalize docking layout.
            _dockStrategy.End();

            // Remove obsolete FloatWindows.
            _dockControl.UpdateFloatWindows();

            Mouse.OverrideCursor = null;

            Reset();
        }


        private void Commit()
        {
            if (_layoutChanged)
            {
                if (IsDraggingDockTabItems)
                {
                    // Dragging ended in DockTabPanel.
                    RestoreFloatWindowPosition();
                }
                else if (_floatWindow != null)
                {
                    // Dragging ended outside of a DockTabPanel. Check the dock indicators to find the
                    // desired target position.
                    IDockPane target = null;
                    DockPosition position = DockPosition.None;
                    if (HasResult(_borderDockIndicators))
                    {
                        target = _dockStrategy.DockControl.RootPane;
                        position = _borderDockIndicators.Result;
                    }
                    else if (HasResult(_paneDockIndicators))
                    {
                        target = DockHelper.GetViewModel<IDockPane>(_paneDockIndicators.Target);
                        position = _paneDockIndicators.Result;
                    }

                    if (position != DockPosition.None && target != null)
                    {
                        // User has dropped FloatWindow on a DockIndicator.
                        // --> Dock content.
                        var floatWindowVM = _floatWindow.GetViewModel();
                        foreach (var item in _draggedItems)
                        {
                            DockHelper.Remove(floatWindowVM, item);
                            item.DockState = DockState.Hide;
                        }

                        var dockTabPane = _dockStrategy.CreateDockTabPane(_draggedItems[0], DockState.Hide);
                        for (int i = 1; i < _draggedItems.Count; i++)
                            dockTabPane.Items.Add(_draggedItems[i]);

                        _dockStrategy.Dock(dockTabPane, target, position);
                        RestoreFloatWindowPosition();
                    }
                    else
                    {
                        // The final state is DockState.Float.
                        if (!_canFloat && _originalDockState != DockState.Float)
                        {
                            // DockState.Float is not allowed.
                            Rollback();
                            return;
                        }
                    }
                }
            }

            // Keep the items at their new position.
            // --> Remove the item proxies.
            var dockState = _draggedItems[0].DockState;
            RemoveItemProxies(dockState);
            if (_layoutChanged && dockState == DockState.Dock)
            {
                // The position within the DockControl may have changed. The assignment to the
                // auto-hide bar is no longer valid.
                // --> Also remove item proxies from auto-hide bars.
                RemoveItemProxies(DockState.AutoHide);
            }

            // Restore the original dock state of the dragged items.
            RestoreItemsFromProxies();
        }


        private void Rollback()
        {
            RestoreFloatWindowPosition();

            if (_layoutChanged)
            {
                // The docking layout has changed.
                // --> Remove the dragged items from their current location.
                switch (_draggedItems[0].DockState)
                {
                    case DockState.Dock:
                        foreach (var item in _draggedItems)
                            DockHelper.Remove(_dockStrategy.DockControl, item);
                        break;
                    case DockState.Float:
                        foreach (var item in _draggedItems)
                            foreach (var floatWindow in _dockStrategy.DockControl.FloatWindows)
                                DockHelper.Remove(floatWindow, item);
                        break;
                    case DockState.AutoHide:
                        foreach (var item in _draggedItems)
                        {
                            DockHelper.Remove(_dockStrategy.DockControl.AutoHideLeft, item);
                            DockHelper.Remove(_dockStrategy.DockControl.AutoHideRight, item);
                            DockHelper.Remove(_dockStrategy.DockControl.AutoHideTop, item);
                            DockHelper.Remove(_dockStrategy.DockControl.AutoHideBottom, item);
                        }
                        break;
                }
            }
            else
            {
                // The docking layout is unchanged.
                // --> Keep the dragged items and remove the item proxies.
                Debug.Assert(_originalDockState == _draggedItems[0].DockState);
                RemoveItemProxies(_draggedItems[0].DockState);
            }

            // Restore items and remove item proxies.
            RestoreItemsFromProxies();

            // Restore the original dock state of the dragged items.
            foreach (var item in _draggedItems)
                item.DockState = _originalDockState;
        }


        private void BackupFloatWindowPosition()
        {
            if (_floatWindow != null)
            {
                _originalFloatWindow = _floatWindow.GetViewModel();
                _originalFloatLeft = _originalFloatWindow.Left;
                _originalFloatTop = _originalFloatWindow.Top;
            }
        }


        private void RestoreFloatWindowPosition()
        {
            if (_originalFloatWindow != null)
            {
                _originalFloatWindow.Left = _originalFloatLeft;
                _originalFloatWindow.Top = _originalFloatTop;
            }
        }

        private DockIndicatorOverlay _borderDockIndicators;
        private DockIndicatorOverlay _paneDockIndicators;


        private void UpdateDockIndicators()
        {
            var targetPane = GetTargetPane();

            // Show/hide dock indicators.
            UpdatePaneIndicators(targetPane);
            UpdateBorderIndicators(targetPane); // Show BorderIndicator on top of PaneIndicators.

            // Hit-test dock indicators in z-order: BorderIndicators before PaneIndicators
            var result = DockPosition.None;
            if (_borderDockIndicators != null)
                result = _borderDockIndicators.HitTest();

            if (_paneDockIndicators != null)
            {
                if (result == DockPosition.None)
                    _paneDockIndicators.HitTest();
                else
                    _paneDockIndicators.ClearResult();
            }
        }


        private void UpdatePaneIndicators(FrameworkElement targetElement)
        {
            if (targetElement != null)
                ShowPaneIndicators(targetElement);
            else
                HidePaneIndicators();
        }


        private void ShowPaneIndicators(FrameworkElement targetElement)
        {
            Debug.Assert(targetElement != null);

            if (_paneDockIndicators != null && _paneDockIndicators.Target == targetElement)
                return;

            HidePaneIndicators();

            // The visible drop target buttons are determined by the DockStrategy.
            // For DockAnchorPanes only the inside button is visible.
            bool isAnchorPane = targetElement is DockAnchorPane;

            _paneDockIndicators = new PaneIndicators(targetElement)
            {
                AllowDockLeft = !isAnchorPane && CanDock(DockHelper.GetViewModel<IDockPane>(targetElement), DockPosition.Left),
                AllowDockTop = !isAnchorPane && CanDock(DockHelper.GetViewModel<IDockPane>(targetElement), DockPosition.Top),
                AllowDockRight = !isAnchorPane && CanDock(DockHelper.GetViewModel<IDockPane>(targetElement), DockPosition.Right),
                AllowDockBottom = !isAnchorPane && CanDock(DockHelper.GetViewModel<IDockPane>(targetElement), DockPosition.Bottom),
                AllowDockInside = CanDock(DockHelper.GetViewModel<IDockPane>(targetElement), DockPosition.Inside),
            };
            _paneDockIndicators.Show();
        }


        private void HidePaneIndicators()
        {
            if (_paneDockIndicators != null)
            {
                _paneDockIndicators.Close();
                _paneDockIndicators = null;
            }
        }


        private void UpdateBorderIndicators(FrameworkElement targetPane)
        {
            // Show BorderIndicators if the mouse is over the DockControl and the targetPane is
            // not in a FloatWindow.
            if (IsMouseOver(_dockControl) && (targetPane == null || !(Window.GetWindow(targetPane) is FloatWindow)))
                ShowBorderDockIndicators();
            else
                HideBorderDockIndicators();
        }


        private void ShowBorderDockIndicators()
        {
            Debug.Assert(_dockControl != null);

            if (_borderDockIndicators == null)
            {
                // The visible drop target buttons are determined by the DockStrategy.
                _borderDockIndicators = new BorderIndicators(_dockControl)
                {
                    AllowDockInside = false,
                    AllowDockLeft = CanDock(null, DockPosition.Left),
                    AllowDockTop = CanDock(null, DockPosition.Top),
                    AllowDockRight = CanDock(null, DockPosition.Right),
                    AllowDockBottom = CanDock(null, DockPosition.Bottom),
                };
                _borderDockIndicators.Show();
            }

            // Bring border DockIndicators to front, otherwise they might be behind the
            // previews (semi-transparent rectangles) of the pane DockIndicators.
            var hWnd = new WindowInteropHelper(_borderDockIndicators).Handle;
            Win32.SetWindowPos(hWnd, IntPtr.Zero, 0, 0, 0, 0, SetWindowPosFlags.SWP_NOMOVE | SetWindowPosFlags.SWP_NOSIZE | SetWindowPosFlags.SWP_SHOWWINDOW | SetWindowPosFlags.SWP_NOACTIVATE);

            // Alternative:
            //_borderDockIndicators.Topmost = true;
            //_borderDockIndicators.Topmost = false;
        }


        private void HideBorderDockIndicators()
        {
            if (_borderDockIndicators != null)
            {
                _borderDockIndicators.Close();
                _borderDockIndicators = null;
            }
        }


        private static bool HasResult(DockIndicatorOverlay dockIndicators)
        {
            return dockIndicators != null && dockIndicators.Result != DockPosition.None;
        }

        internal void OnFloatWindowEnterMove(FloatWindow floatWindow)
        {
            Debug.Assert(floatWindow != null);

            if (!IsDragging)
                BeginDrag(floatWindow, null, null);
        }


        internal void OnFloatWindowMove(FloatWindow floatWindow)
        {
            if (IsDraggingFloatWindow)
            {
                Debug.Assert(_floatWindow == floatWindow);

                Drag();
            }
        }


        internal void OnFloatWindowExitMove(FloatWindow floatWindow)
        {
            if (IsDraggingFloatWindow)
            {
                Debug.Assert(_floatWindow == floatWindow);

                EndDrag(!Keyboard.IsKeyDown(Key.Escape));
            }
        }


        /// <summary>
        /// Called when <see cref="DockTabPane"/>s are added to or removed from to the
        /// <see cref="DockControl"/>.
        /// </summary>
        private void OnDockTabPanesChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            if (eventArgs.OldItems != null)
                foreach (var dockTabPane in eventArgs.OldItems.OfType<DockTabPane>())
                    dockTabPane.MouseLeftButtonDown -= OnDockTabPaneMouseDown;

            if (eventArgs.NewItems != null)
                foreach (var dockTabPane in eventArgs.NewItems.OfType<DockTabPane>())
                    dockTabPane.MouseLeftButtonDown += OnDockTabPaneMouseDown;
        }


        private void OnDockTabPaneMouseDown(object sender, MouseButtonEventArgs eventArgs)
        {
            if (eventArgs.Handled || IsDragging)
                return;

            var dockTabPane = (DockTabPane)sender;

            // Make sure the DockTabPane is active. (Clicks on the tab are automatically handled in
            // DockTabItem. But clicks inside of the window need to be handled here.)
            var selectedDockTabItem = dockTabPane.ItemContainerGenerator.ContainerFromIndex(dockTabPane.SelectedIndex) as DockTabItem;
            selectedDockTabItem?.Activate();

            // Start dragging.
            var dockTabItem = GetDockTabItemAtMouse(dockTabPane);
            bool dockTabPaneClicked = (eventArgs.OriginalSource as FrameworkElement)?.Name == "PART_DragArea";
            bool dockTabItemClicked = dockTabItem != null;

            var floatWindow = Window.GetWindow(dockTabPane) as FloatWindow;
            var rootPane = floatWindow?.GetViewModel()?.RootPane as IDockTabPane;
            if (rootPane != null && (dockTabPaneClicked || dockTabItemClicked && dockTabPane.Items.Count == 1))
            {
                // The FloatWindow contains a single DockTabPane and the DockTabPane was clicked.
                // Or, the FloatWindow contains a single DockTabItem and the DockTabItem was clicked.
                // --> Drag entire FloatWindow.

                // If FloatWindow is maximized, then minimize it and move it at the mouse position.
                if (floatWindow.WindowState == WindowState.Maximized)
                {
                    _mouseOffset = eventArgs.GetPosition(floatWindow) - new Point();
                    floatWindow.WindowState = WindowState.Normal;
                    Point position = GetFloatWindowPosition();
                    floatWindow.Left = position.X;
                    floatWindow.Top = position.Y;
                }

                // Call Window.DragMove() to trigger the Win32 move window loop.
                floatWindow.DragMove();
            }
            else if (dockTabPaneClicked)
            {
                // --> Drag entire DockTabPane.
                eventArgs.Handled = BeginDrag(null, dockTabPane, null);
            }
            else if (dockTabItemClicked)
            {
                // --> Drag DockTabItem tab inside DockTabPanel.
                eventArgs.Handled = BeginDrag(null, dockTabPane, dockTabItem);
            }
        }


        private void OnLostMouseCapture(object sender, MouseEventArgs eventArgs)
        {
            Debug.Assert(IsDraggingDockTabItems);

            EndDrag(false);
        }


        private void OnMouseLeftButtonUp(object sender, MouseEventArgs eventArgs)
        {
            Debug.Assert(IsDraggingDockTabItems);

            EndDrag(true);
        }


        private void OnMouseMove(object sender, MouseEventArgs eventArgs)
        {
            Debug.Assert(IsDraggingDockTabItems);

            Drag();
        }


        private void OnPreviewKeyDown(object sender, KeyEventArgs eventArgs)
        {
            Debug.Assert(IsDraggingDockTabItems);

            if (eventArgs.Key == Key.Escape)
            {
                EndDrag(false);
                eventArgs.Handled = true;
            }
        }

        private bool CanDock(IDockPane target, DockPosition position)
        {
            Debug.Assert(_dockStrategy != null);
            Debug.Assert(_draggedItems.Count > 0);

            target = target ?? _dockStrategy.DockControl.RootPane;

            foreach (var item in _draggedItems)
                if (!_dockStrategy.CanDock(item, target, position))
                    return false;

            return true;
        }


        /// <summary>
        /// Determines whether a <see cref="FloatWindow"/> is valid as the final dock state.
        /// </summary>
        /// <returns>
        /// <see langword="true"/> the dragged items may be shown in a <see cref="FloatWindow"/>;
        /// otherwise, <see langword="false"/>.
        /// </returns>
        private bool CanFloat()
        {
            Debug.Assert(_dockStrategy != null);
            Debug.Assert(_draggedItems.Count > 0);

            foreach (var item in _draggedItems)
                if (item.DockState != DockState.Float && !_dockStrategy.CanFloat(item))
                    return false;

            return true;
        }


        /// <summary>
        /// Gets the <see cref="FloatWindow"/> (view) for the <see cref="IFloatWindow"/>
        /// (view-model).
        /// </summary>
        /// <param name="viewModel">The <see cref="IFloatWindow"/>.</param>
        /// <returns>The <see cref="FloatWindow"/>.</returns>
        private FloatWindow GetFloatWindow(IFloatWindow viewModel)
        {
            Debug.Assert(_dockControl != null);
            Debug.Assert(viewModel != null);

            _dockControl.UpdateFloatWindows();
            for (int i = 0; i < _dockControl.FloatWindows.Count; i++)
            {
                var floatWindow = _dockControl.FloatWindows[i];
                if (floatWindow.GetViewModel() == viewModel)
                    return floatWindow;
            }

            return null;
        }


        /// <summary>
        /// Gets the position of a <see cref="FloatWindow"/> if one would be shown at the current
        /// mouse position.
        /// </summary>
        /// <returns>The <see cref="FloatWindow"/> position.</returns>
        private Point GetFloatWindowPosition()
        {
            Debug.Assert(_dockControl != null);

            // Mouse position relative to DockControl in device-independent pixels.
            Point position = WpfWindowsExtensions.GetMousePosition(_dockControl);

            // Absolute mouse position in native screen coordinates.
            position = _dockControl.PointToScreen(position);

            // Absolute mouse position in device-independent pixels.
            position = DockHelper.ScreenToLogical(_dockControl, position);

            // Offset between window origin and mouse cursor.
            position -= _mouseOffset;

            return position;
        }


        /// <summary>
        /// Limits the extent of the <see cref="FloatWindow"/> to a reasonable size.
        /// </summary>
        /// <param name="floatWindow">The <see cref="FloatWindow"/>.</param>
        /// <param name="fallbackSize">The fallback size. Can be (NaN, NaN).</param>
        private static void LimitFloatWindowSize(FloatWindow floatWindow, Size fallbackSize)
        {
            Debug.Assert(floatWindow != null);
            Debug.Assert(Numeric.IsNaN(fallbackSize.Width) || fallbackSize.Width > 0);
            Debug.Assert(Numeric.IsNaN(fallbackSize.Height) || fallbackSize.Height > 0);

            double maxWidth = SystemParameters.MaximizedPrimaryScreenWidth;
            double maxHeight = SystemParameters.MaximizedPrimaryScreenHeight;

            Size size = floatWindow.RenderSize;
            if (size.Width > maxWidth)
                floatWindow.Width = Numeric.IsNaN(fallbackSize.Width) ? maxWidth : fallbackSize.Width;
            if (size.Height > maxHeight)
                floatWindow.Height = Numeric.IsNaN(fallbackSize.Height) ? maxHeight : fallbackSize.Height;
        }


        /// <summary>
        /// Gets the <see cref="DockTabItem"/> at the mouse position by testing against the
        /// <see cref="DockTabItem"/> tabs in the specified pane.
        /// </summary>
        /// <param name="dockTabPane">The <see cref="DockTabPane"/>.</param>
        /// <param name="verticalTolerance">
        /// The tolerance (margin at top and bottom) in pixels.
        /// </param>
        /// <returns>The <see cref="DockTabItem"/> under the mouse cursor.</returns>
        private static DockTabItem GetDockTabItemAtMouse(DockTabPane dockTabPane, double verticalTolerance = 0)
        {
            Debug.Assert(dockTabPane != null);

            if (dockTabPane.Items.Count == 0)
                return null;    // Empty DockTabPane.

            var itemsPanel = dockTabPane.GetItemsPanel();
            if (itemsPanel == null)
                return null;    // ItemsPanel missing.

            Point mousePosition = WpfWindowsExtensions.GetMousePosition(itemsPanel);

            Rect bounds = new Rect(itemsPanel.RenderSize);
            bounds.Inflate(0, verticalTolerance);
            if (!bounds.Contains(mousePosition))
                return null;    // Mouse position outside ItemsPanel.

            // Test mouse position against DockTabItems bounds.
            double height = itemsPanel.RenderSize.Height;
            double x = 0;
            for (int i = 0; i < dockTabPane.Items.Count; i++)
            {
                var dockTabItem = dockTabPane.ItemContainerGenerator.ContainerFromIndex(i) as DockTabItem;
                if (dockTabItem == null)
                    break;

                bounds = new Rect(new Point(x, 0), new Size(dockTabItem.RenderSize.Width, height));
                bounds.Inflate(0, verticalTolerance);
                if (bounds.Contains(mousePosition))
                    return dockTabItem;

                x += bounds.Width;
            }

            return null;
        }


        /// <summary>
        /// Applies a horizontal offset to the dragged <see cref="DockTabItem"/>s.
        /// </summary>
        private void SetTranslateTransform()
        {
            Debug.Assert(_targetDockTabPane != null);

            double? offset = null;
            for (int i = 0; i < _targetDockTabPane.Items.Count; i++)
            {
                var dockTabItem = _targetDockTabPane.ItemContainerGenerator.ContainerFromIndex(i) as DockTabItem;

                if (dockTabItem == null)
                    continue;   // Item container not yet generated.

                if (!_draggedItems.Contains(dockTabItem.GetViewModel()))
                    continue;   // Item is not dragged.

                if (offset == null)
                {
                    // The mouse cursor should be on the first dragged DockTabItem.
                    if (_mouseOffset.X < 0 || _mouseOffset.X > dockTabItem.RenderSize.Width)
                    {
                        if (dockTabItem.RenderSize.Width > 0)
                            _mouseOffset.X = dockTabItem.RenderSize.Width / 4;
                        else
                            _mouseOffset.X = 32;    // Item not yet measured. Use default value.
                    }

                    Point mousePosition = WpfWindowsExtensions.GetMousePosition(dockTabItem);
                    offset = mousePosition.X - _mouseOffset.X;
                }

                // Set DockTabPanel.IsDragged flag. Items which are dragged are not animated
                // by the DockTabPanel.
                DockTabPanel.SetIsDragged(dockTabItem, true);

                // Apply offset as RenderTransform.
                var translateTransform = dockTabItem.RenderTransform as TranslateTransform;
                if (translateTransform == null)
                {
                    translateTransform = new TranslateTransform();
                    dockTabItem.RenderTransform = translateTransform;
                }

                translateTransform.X += offset.Value;
            }
        }


        /// <summary>
        /// Clears the horizontal offset from the dragged <see cref="DockTabItem"/>s.
        /// </summary>
        private void ClearTranslateTransform()
        {
            Debug.Assert(_targetDockTabPane != null);

            for (int i = 0; i < _targetDockTabPane.Items.Count; i++)
            {
                var dockTabItem = _targetDockTabPane.ItemContainerGenerator.ContainerFromIndex(i) as DockTabItem;
                if (dockTabItem != null && DockTabPanel.GetIsDragged(dockTabItem))
                {
                    DockTabPanel.SetIsDragged(dockTabItem, false);
                    var translateTransform = dockTabItem.RenderTransform as TranslateTransform;
                    if (translateTransform != null)
                        translateTransform.X = 0;
                }
            }
        }


        /// <summary>
        /// Determines whether the mouse cursor is over the specified element.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <param name="verticalTolerance">
        /// The tolerance (margin at top and bottom) in pixels.
        /// </param>
        /// <returns>
        /// <see langword="true"/> if the mouse is over <paramref name="element"/>; otherwise,
        /// <see langword="false"/>.
        /// </returns>
        private static bool IsMouseOver(UIElement element, double verticalTolerance = 0)
        {
            Debug.Assert(element != null);

            Rect bounds = new Rect(element.RenderSize);
            bounds.Inflate(0, verticalTolerance);
            Point mousePosition = WpfWindowsExtensions.GetMousePosition(element);
            return bounds.Contains(mousePosition);
        }


        /// <summary>
        /// Gets the dock pane under the mouse cursor.
        /// </summary>
        /// <returns>The dock pane under the mouse cursor.</returns>
        /// <remarks>This method ignores the currently dragged floating window.</remarks>
        private FrameworkElement GetTargetPane()
        {
            Debug.Assert(_dockControl != null);

            FrameworkElement targetPane = null;
            bool floatWindowHit = false;

            // Check whether mouse is above one of the other FloatWindows.
            // (Iterate from top to bottom window.)
            var floatWindows = _dockControl.FloatWindows.OrderByDescending(window => window.LastActivation);
            foreach (var floatWindow in floatWindows)
            {
                Debug.Assert(floatWindow != null, "DockControl.FloatWindows must not contain null.");

                if (floatWindow == _floatWindow || !floatWindow.IsVisible)
                    continue;

                floatWindowHit = IsMouseOver(floatWindow);
                if (floatWindowHit)
                {
                    targetPane = GetTargetPane(floatWindow);
                    if (targetPane != null)
                        break;
                }
            }

            // Reject targetPane if _canFloat is false because we are not allowed to drag the items
            // into a FloatWindow.
            if (targetPane != null && !_canFloat)
                return null;

            if (!floatWindowHit)
                targetPane = GetTargetPane(_dockControl);

            return targetPane;
        }


        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        private static FrameworkElement GetTargetPane(FrameworkElement element)
        {
            Debug.Assert(element != null);

            if (element is DockControl)
            {
                var dockControl = (DockControl)element;
                var rootPane = dockControl.RootPane;
                if (rootPane != null)
                    return GetTargetPane(rootPane);
            }
            else if (element is FloatWindow)
            {
                var floatWindow = (FloatWindow)element;
                var rootPane = floatWindow.RootPane;
                if (rootPane != null)
                    return GetTargetPane(rootPane);
            }
            else if (element is DockAnchorPane)
            {
                var dockAnchorPane = (DockAnchorPane)element;
                var childPane = dockAnchorPane.ChildPane;
                if (childPane != null)
                    return GetTargetPane(childPane);
                if (IsMouseOver(dockAnchorPane))
                    return dockAnchorPane;
            }
            else if (element is DockSplitPane)
            {
                var dockSplitPane = (DockSplitPane)element;
                for (int i = 0; i < dockSplitPane.Items.Count; i++)
                {
                    var childPane = dockSplitPane.ItemContainerGenerator.ContainerFromIndex(i) as FrameworkElement;
                    if (childPane == null)
                        continue;

                    var targetPane = GetTargetPane(childPane);
                    if (targetPane != null)
                        return targetPane;
                }
            }
            else if (element is DockTabPane)
            {
                var dockTabPane = (DockTabPane)element;
                if (IsMouseOver(dockTabPane))
                    return dockTabPane;
            }

            return null;
        }

        private void ReplaceItemsWithProxies(IDockTabPane currentPane)
        {
            Debug.Assert(_dockStrategy != null);

            ReplaceItemsWithProxies(_dockStrategy.DockControl.RootPane, _draggedItems, currentPane);

            foreach (var floatWindow in _dockStrategy.DockControl.FloatWindows)
                ReplaceItemsWithProxies(floatWindow.RootPane, _draggedItems, currentPane);

            ReplaceItemsWithProxies(_dockStrategy.DockControl.AutoHideLeft, _draggedItems, currentPane);
            ReplaceItemsWithProxies(_dockStrategy.DockControl.AutoHideRight, _draggedItems, currentPane);
            ReplaceItemsWithProxies(_dockStrategy.DockControl.AutoHideTop, _draggedItems, currentPane);
            ReplaceItemsWithProxies(_dockStrategy.DockControl.AutoHideBottom, _draggedItems, currentPane);
        }


        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        private static void ReplaceItemsWithProxies(IDockPane dockPane, List<IDockTabItem> items, IDockTabPane currentPane)
        {
            Debug.Assert(dockPane != null);
            Debug.Assert(items != null);

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane != null)
                    ReplaceItemsWithProxies(dockAnchorPane.ChildPane, items, currentPane);
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                ReplaceItemsWithProxies(dockSplitPane.ChildPanes, items, currentPane);
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                if (dockTabPane == currentPane)
                {
                    // Special: If the IDockTabPane is the current pane, add the item proxies but
                    // keep the original items.
                    for (int i = 0; i < dockTabPane.Items.Count; i++)
                    {
                        if (items.IndexOf(dockTabPane.Items[i]) >= 0)
                        {
                            dockTabPane.Items.Insert(i + 1, new DockTabItemProxy(dockTabPane.Items[i]));
                            i++;
                        }
                    }
                }
                else
                {
                    for (int i = 0; i < dockTabPane.Items.Count; i++)
                    {
                        if (items.IndexOf(dockTabPane.Items[i]) >= 0)
                            dockTabPane.Items[i] = new DockTabItemProxy(dockTabPane.Items[i]);
                    }
                }
            }
        }


        private static void ReplaceItemsWithProxies(IReadOnlyList<IDockPane> dockPanes, List<IDockTabItem> items, IDockTabPane currentPane)
        {
            Debug.Assert(dockPanes != null);
            Debug.Assert(items != null);

            for (int i = 0; i < dockPanes.Count; i++)
                ReplaceItemsWithProxies(dockPanes[i], items, currentPane);
        }


        /// <summary>
        /// Removes the item proxies from the specified <see cref="IDockPane"/>.
        /// </summary>
        /// <param name="dockPane">The <see cref="IDockPane"/>.</param>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        private static void RemoveItemProxies(IDockPane dockPane)
        {
            Debug.Assert(dockPane != null);

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane != null)
                    RemoveItemProxies(dockAnchorPane.ChildPane);
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                RemoveItemProxies(dockSplitPane.ChildPanes);
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                for (int i = dockTabPane.Items.Count - 1; i >= 0; i--)
                {
                    if (dockTabPane.Items[i] is DockTabItemProxy)
                        dockTabPane.Items.RemoveAt(i);
                }
            }
        }


        /// <summary>
        /// Removes the item proxies with the specified dock state.
        /// </summary>
        /// <param name="dockState">The dock state.</param>
        private void RemoveItemProxies(DockState dockState)
        {
            switch (dockState)
            {
                case DockState.Dock:
                    RemoveItemProxies(_dockStrategy.DockControl.RootPane);
                    break;
                case DockState.Float:
                    foreach (var floatWindow in _dockStrategy.DockControl.FloatWindows)
                        RemoveItemProxies(floatWindow.RootPane);
                    break;
                case DockState.AutoHide:
                    RemoveItemProxies(_dockStrategy.DockControl.AutoHideLeft);
                    RemoveItemProxies(_dockStrategy.DockControl.AutoHideRight);
                    RemoveItemProxies(_dockStrategy.DockControl.AutoHideTop);
                    RemoveItemProxies(_dockStrategy.DockControl.AutoHideBottom);
                    break;
            }
        }


        /// <summary>
        /// Removes the item proxies from the specified list of <see cref="IDockPane"/>s.
        /// </summary>
        /// <param name="dockPanes">The list of <see cref="IDockPane"/>s.</param>
        private static void RemoveItemProxies(IReadOnlyList<IDockPane> dockPanes)
        {
            Debug.Assert(dockPanes != null);

            for (int i = 0; i < dockPanes.Count; i++)
                RemoveItemProxies(dockPanes[i]);
        }


        /// <summary>
        /// Restores the items from their item proxies.
        /// </summary>
        private void RestoreItemsFromProxies()
        {
            Debug.Assert(_dockStrategy != null);

            RestoreItemsFromProxies(_dockStrategy.DockControl.RootPane);

            foreach (var floatWindow in _dockStrategy.DockControl.FloatWindows)
                RestoreItemsFromProxies(floatWindow.RootPane);

            RestoreItemsFromProxies(_dockStrategy.DockControl.AutoHideLeft);
            RestoreItemsFromProxies(_dockStrategy.DockControl.AutoHideRight);
            RestoreItemsFromProxies(_dockStrategy.DockControl.AutoHideTop);
            RestoreItemsFromProxies(_dockStrategy.DockControl.AutoHideBottom);
        }


        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        private static void RestoreItemsFromProxies(IDockPane dockPane)
        {
            Debug.Assert(dockPane != null);

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane != null)
                    RestoreItemsFromProxies(dockAnchorPane.ChildPane);
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                RestoreItemsFromProxies(dockSplitPane.ChildPanes);
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                for (int i = dockTabPane.Items.Count - 1; i >= 0; i--)
                {
                    var proxy = dockTabPane.Items[i] as DockTabItemProxy;
                    if (proxy != null)
                        dockTabPane.Items[i] = proxy.Item;
                }
            }
        }


        private static void RestoreItemsFromProxies(IReadOnlyList<IDockPane> dockPanes)
        {
            Debug.Assert(dockPanes != null);

            for (int i = 0; i < dockPanes.Count; i++)
                RestoreItemsFromProxies(dockPanes[i]);
        }
    }
}
