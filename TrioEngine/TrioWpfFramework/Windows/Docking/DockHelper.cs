using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace TrioWpfFramework.Windows.Docking
{
    public static class DockHelper
    {
        internal static readonly GridLength GridLengthOneStar = new GridLength(1, GridUnitType.Star);
        internal static readonly object BoxedGridLengthOneStar = GridLengthOneStar;
        internal static readonly object BoxedDockStateDocked = DockState.Dock;
        internal static readonly object BoxedDockStateFloating = DockState.Float;

        public static IDockPane GetParent(IDockControl dockControl, IDockElement child)
        {
            if (dockControl == null)
                throw new ArgumentNullException(nameof(dockControl));
            if (child == null)
                throw new ArgumentNullException(nameof(child));

            Debug.Assert(child != null);
            Debug.Assert(child.DockState != DockState.Hide);

            switch (child.DockState)
            {
                case DockState.Dock:
                    return GetParent(dockControl.RootPane, child);
                case DockState.Float:
                    foreach (var floatWindow in dockControl.FloatWindows)
                    {
                        var dockPane = GetParent(floatWindow.RootPane, child);
                        if (dockPane != null)
                            return dockPane;
                    }
                    break;
                case DockState.AutoHide:
                    return GetParent(dockControl.AutoHideLeft, child)
                           ?? GetParent(dockControl.AutoHideRight, child)
                           ?? GetParent(dockControl.AutoHideTop, child)
                           ?? GetParent(dockControl.AutoHideBottom, child);
            }

            return null;
        }

        public static IDockPane GetParent(IDockPane dockPane, IDockElement child)
        {
            Debug.Assert(dockPane != null);
            Debug.Assert(child != null);

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane == child)
                    return dockAnchorPane;

                if (dockAnchorPane.ChildPane != null)
                    return GetParent(dockAnchorPane.ChildPane, child);
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                var childPane = child as IDockPane;
                if (childPane != null && dockSplitPane.ChildPanes.Contains(childPane))
                    return dockSplitPane;

                return GetParent(dockSplitPane.ChildPanes, child);
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                var dockTabItem = child as IDockTabItem;
                if (dockTabItem != null && dockTabPane.Items.Contains(dockTabItem))
                    return dockTabPane;
            }

            return null;
        }

        internal static IDockPane GetParent(IReadOnlyList<IDockPane> dockPanes, IDockElement child)
        {
            Debug.Assert(dockPanes != null);
            Debug.Assert(child != null);

            for (int i = 0; i < dockPanes.Count; i++)
            {
                var parent = GetParent(dockPanes[i], child);
                if (parent != null)
                    return parent;
            }

            return null;
        }

        internal static T GetContentContainer<T>(this ContentPresenter contentPresenter) where T : UIElement
        {
            if (contentPresenter != null
                && contentPresenter.Visibility == Visibility.Visible    // Needed for DockAnchorPane.
                && VisualTreeHelper.GetChildrenCount(contentPresenter) > 0)
            {
                return VisualTreeHelper.GetChild(contentPresenter, 0) as T;
            }

            return null;
        }

        internal static DockControl GetDockControl(DependencyObject element)
        {
            while (element != null)
            {
                var dockControl = element as DockControl;
                if (dockControl != null)
                    return dockControl;

                var floatWindow = element as FloatWindow;
                if (floatWindow != null)
                    return floatWindow.DockControl;

                var autoHideOverlay = element as AutoHideOverlay;
                if (autoHideOverlay != null)
                    return autoHideOverlay.DockControl;

                element = VisualTreeHelper.GetParent(element);
            }

            return null;
        }

        internal static DockState GetDockState(DependencyObject element)
        {
            while (element != null)
            {
                if (element is DockControl)
                    return DockState.Dock;

                if (element is FloatWindow)
                    return DockState.Float;

                if (element is AutoHidePane || element is AutoHideBar)
                    return DockState.AutoHide;

                element = VisualTreeHelper.GetParent(element);
            }

            return DockState.Hide;
        }

        /// <summary>
        /// Converts native screen coordinates to device-independent pixels (logical units).
        /// (Necessary if DPI scaling is enabled in Windows.)
        /// </summary>
        internal static Point ScreenToLogical(Visual visual, Point position)
        {
            Debug.Assert(visual != null);

            var compositionTarget = PresentationSource.FromVisual(visual)?.CompositionTarget;
            if (compositionTarget != null)
                position = compositionTarget.TransformFromDevice.Transform(position);

            return position;
        }

        internal static T GetViewModel<T>(FrameworkElement view) where T : class
        {
            Debug.Assert(view != null);
            return view.DataContext as T;
        }


        internal static IDockControl GetViewModel(this DockControl view)
        {
            return GetViewModel<IDockControl>(view);
        }


        internal static IFloatWindow GetViewModel(this FloatWindow view)
        {
            return GetViewModel<IFloatWindow>(view);
        }


        internal static IDockTabPane GetViewModel(this DockTabPane view)
        {
            return GetViewModel<IDockTabPane>(view);
        }


        internal static IDockTabItem GetViewModel(this DockTabItem view)
        {
            return GetViewModel<IDockTabItem>(view);
        }

        public static IEnumerable<IDockElement> GetDockElements(this IDockControl dockControl)
        {
            if (dockControl == null)
                throw new ArgumentNullException(nameof(dockControl));

            var dockElements = GetDockElements(dockControl.RootPane);
            var floatElements = dockControl.FloatWindows.SelectMany(floatWindow => GetDockElements(floatWindow.RootPane));
            var autoHideLeftElements = GetDockElements(dockControl.AutoHideLeft);
            var autoHideTopElements = GetDockElements(dockControl.AutoHideTop);
            var autoHideRightElements = GetDockElements(dockControl.AutoHideRight);
            var autoHideBottomElements = GetDockElements(dockControl.AutoHideBottom);

            return dockElements.Concat(floatElements)
                               .Concat(autoHideLeftElements)
                               .Concat(autoHideTopElements)
                               .Concat(autoHideRightElements)
                               .Concat(autoHideBottomElements);
        }

        public static IEnumerable<IDockElement> GetDockElements(this IDockPane dockPane)
        {
            if (dockPane == null)
                throw new ArgumentNullException(nameof(dockPane));

            yield return dockPane;

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane != null)
                    foreach (var dockElement in GetDockElements(dockAnchorPane.ChildPane))
                        yield return dockElement;
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                foreach (var dockElement in GetDockElements(dockSplitPane.ChildPanes))
                    yield return dockElement;
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                foreach (var dockTabItem in dockTabPane.Items)
                    yield return dockTabItem;
            }
        }

        public static IEnumerable<IDockElement> GetDockElements(this IReadOnlyList<IDockPane> dockPanes)
        {
            if (dockPanes == null)
                throw new ArgumentNullException(nameof(dockPanes));

            return dockPanes.SelectMany(GetDockElements);
        }

        internal static bool Contains(this IDockContainer dockContainer, IDockElement dockElement)
        {
            Debug.Assert(dockContainer != null);
            Debug.Assert(dockElement != null);

            if (dockContainer.RootPane == null)
                return false;

            return Contains(dockContainer.RootPane, dockElement);
        }

        internal static bool Contains(this IDockPane dockPane, IDockElement dockElement)
        {
            Debug.Assert(dockPane != null);
            Debug.Assert(dockElement != null);

            if (dockPane == dockElement)
                return true;

            if (dockPane is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)dockPane;
                if (dockAnchorPane.ChildPane != null)
                    return Contains(dockAnchorPane.ChildPane, dockElement);
            }
            else if (dockPane is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)dockPane;
                if (Contains(dockSplitPane.ChildPanes, dockElement))
                    return true;
            }
            else if (dockPane is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockPane;
                var dockTabItem = dockElement as IDockTabItem;
                if (dockTabItem != null)
                    return dockTabPane.Items.Contains(dockTabItem);
            }

            return false;
        }

        internal static bool Contains(this IReadOnlyList<IDockPane> dockPanes, IDockElement dockElement)
        {
            Debug.Assert(dockPanes != null);
            Debug.Assert(dockElement != null);

            foreach (var dockPane in dockPanes)
                if (Contains(dockPane, dockElement))
                    return true;

            return false;
        }

        internal static void Remove(IDockContainer rootContainer, IDockElement dockElement)
        {
            Debug.Assert(rootContainer != null);
            Debug.Assert(rootContainer.RootPane != null);
            Debug.Assert(dockElement != null);

            if (rootContainer.RootPane == dockElement)
                rootContainer.RootPane = null;
            else
                Remove(rootContainer.RootPane, dockElement);
        }

        internal static void Remove(IDockPane dockPane, IDockElement dockElement)
        {
            Debug.Assert(dockPane != null);
            Debug.Assert(dockElement != null);
            Debug.Assert(dockPane != dockElement);

            var parent = GetParent(dockPane, dockElement);
            if (parent is IDockAnchorPane)
            {
                Debug.Assert(dockElement is IDockPane);

                var dockAnchorPane = (IDockAnchorPane)parent;
                dockAnchorPane.ChildPane = null;
            }
            else if (parent is IDockSplitPane)
            {
                Debug.Assert(dockElement is IDockPane);

                var dockSplitPane = (IDockSplitPane)parent;
                dockSplitPane.ChildPanes.Remove((IDockPane)dockElement);
            }
            else if (parent is IDockTabPane)
            {
                Debug.Assert(dockElement is IDockTabItem);

                var dockTabPane = (IDockTabPane)parent;
                dockTabPane.Items.Remove((IDockTabItem)dockElement);
            }
        }

        internal static void Remove(DockTabPaneCollection dockTabPanes, IDockElement dockElement)
        {
            Debug.Assert(dockTabPanes != null);
            Debug.Assert(dockElement != null);

            if (dockElement is IDockTabPane)
            {
                var dockTabPane = (IDockTabPane)dockElement;
                dockTabPanes.Remove(dockTabPane);
            }
            else if (dockElement is IDockTabItem)
            {
                var dockTabItem = (IDockTabItem)dockElement;
                var dockTabPane = (IDockTabPane)GetParent(dockTabPanes, dockTabItem);
                dockTabPane?.Items.Remove(dockTabItem);
            }
        }

        internal static bool HitTest(UIElement element)
        {
            var mousePosition = WpfWindowsExtensions.GetMousePosition(element);
            var hitTestResult = VisualTreeHelper.HitTest(element, mousePosition);
            return hitTestResult != null;
        }

        internal static void Replace(IDockPane parent, IDockPane oldChild, IDockPane newChild)
        {
            Debug.Assert(parent != null);
            Debug.Assert(oldChild != null);
            Debug.Assert(newChild != null);
            Debug.Assert(oldChild != newChild);

            if (parent is IDockAnchorPane)
            {
                var dockAnchorPane = (IDockAnchorPane)parent;
                Debug.Assert(dockAnchorPane.ChildPane == oldChild);
                dockAnchorPane.ChildPane = newChild;
            }
            else if (parent is IDockSplitPane)
            {
                var dockSplitPane = (IDockSplitPane)parent;
                int index = dockSplitPane.ChildPanes.IndexOf(oldChild);
                Debug.Assert(index >= -1, "Previous child not found in DockSplitPane.");

                // Important: Use Remove + Insert. Do not use Set (indexer)!
                // When the indexer is used, the ItemsControl reuses the previous item container
                // instead of creating the correct item container based on the data template.
                dockSplitPane.ChildPanes.RemoveAt(index);
                dockSplitPane.ChildPanes.Insert(index, newChild);
            }
        }
    }
}
