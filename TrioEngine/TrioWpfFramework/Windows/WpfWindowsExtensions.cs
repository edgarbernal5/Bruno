using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Threading;
using TrioWpfFramework.Net.Linq;
using TrioWpfFramework.Windows.Interop;

namespace TrioWpfFramework.Windows
{
    public static class WpfWindowsExtensions
    {
        public static DependencyObject GetVisualRoot(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetRoot(dependencyObject, VisualTreeHelper.GetParent);
        }

        public static IEnumerable<DependencyObject> GetVisualDescendants(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetDescendants(dependencyObject, GetVisualChildren, true);
        }

        public static IEnumerable<DependencyObject> GetVisualChildren(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return GetVisualChildrenImpl(dependencyObject);
        }

        private static IEnumerable<DependencyObject> GetVisualChildrenImpl(DependencyObject dependencyObject)
        {
            FrameworkElement frameworkElement = dependencyObject as FrameworkElement;
            if (frameworkElement != null)
                frameworkElement.ApplyTemplate();

            int count = VisualTreeHelper.GetChildrenCount(dependencyObject);
            for (int i = 0; i < count; ++i)
            {
                yield return VisualTreeHelper.GetChild(dependencyObject, i);
            }
        }

        public static IEnumerable<DependencyObject> GetLogicalAncestors(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetAncestors(dependencyObject, GetLogicalParent);
        }

        public static DependencyObject GetLogicalParent(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            DependencyObject parent = LogicalTreeHelper.GetParent(dependencyObject);

            // If the current element is in a template, we have to use TemplatedParent.
            if (parent == null)
            {
                if (dependencyObject is Popup)
                    parent = ((Popup)dependencyObject).PlacementTarget;
                else if (dependencyObject is FrameworkElement)
                    parent = ((FrameworkElement)dependencyObject).TemplatedParent;
                else if (dependencyObject is FrameworkContentElement)
                    parent = ((FrameworkContentElement)dependencyObject).TemplatedParent;
            }

            return parent;
        }

        public static Point GetMousePosition(Visual visual)
        {
            if (visual == null)
                throw new ArgumentNullException(nameof(visual));

            POINT pointNative = new POINT();
            Win32.GetCursorPos(ref pointNative);
            Point point = new Point(pointNative.X, pointNative.Y);

            // Convert mouse position from screen coordinates into local coordinates of visual.
            return visual.PointFromScreen(point);
        }

        public static Panel GetItemsPanel(this ItemsControl itemsControl)
        {
            if (itemsControl == null)
                throw new ArgumentNullException(nameof(itemsControl));

            // First, make sure all elements are loaded.
            itemsControl.ApplyTemplate();

            // Get internal member ItemsControl.ItemsHost using reflection.
            return (Panel)typeof(ItemsControl).InvokeMember(
                "ItemsHost", BindingFlags.NonPublic | BindingFlags.GetProperty | BindingFlags.Instance,
                null, itemsControl, null, CultureInfo.InvariantCulture);

            //// Try to get item panel from item container.
            //if (itemsControl.Items.Count > 0)
            //{
            //    var itemContainer = itemsControl.ItemContainerGenerator.ContainerFromIndex(0);
            //    if (itemContainer != null)
            //        return VisualTreeHelper.GetParent(itemContainer) as Panel;
            //}

            //// Search visual tree.
            //return itemsControl.GetVisualDescendants()
            //                   .OfType<Panel>()
            //                   .FirstOrDefault(panel => ItemsControl.GetItemsOwner(panel) == itemsControl);
        }

        public static bool IsInDesignMode
        {
            get
            {
                // Detect design mode:
                // See http://blogs.msdn.com/jnak/archive/2006/10/07/Detecting-Design-Mode.aspx and
                // http://blog.galasoft.ch/archive/2009/09/05/detecting-design-time-mode-in-wpf-and-silverlight.aspx.
                if (!_isInDesignMode.HasValue)
                {

                    _isInDesignMode = (bool)DependencyPropertyDescriptor.FromProperty(DesignerProperties.IsInDesignModeProperty, typeof(FrameworkElement))
                                                                        .Metadata
                                                                        .DefaultValue;

                    if (!_isInDesignMode.Value)
                    {
                        var processName = Process.GetCurrentProcess().ProcessName.ToUpperInvariant();
                        if (processName.Contains("DEVENV")
                            || processName.Contains("XDESPROC") // VS 2012 XAML Designer
                            || processName.StartsWith("V", StringComparison.Ordinal) && processName.Contains("EXPRESS"))
                        {
                            _isInDesignMode = true;
                        }
                    }
                }

                return _isInDesignMode.Value;
            }
        }
        private static bool? _isInDesignMode;

        /// <summary>
        /// Gets or sets the dispatcher associated with the UI thread.
        /// </summary>
        /// <value>
        /// The <see cref="Dispatcher"/> associated with the UI thread, or <see langword="null"/> if
        /// there is currently no dispatcher associated with the UI thread. A custom dispatcher can
        /// be set to overwrite default dispatcher. (Setting a value of <see langword="null"/>
        /// resets the property to the default dispatcher.)
        /// </value>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="CheckBeginInvokeOnUI"/>
        /// <seealso cref="BeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static Dispatcher Dispatcher

        {
            get
            {
                // Note: Background threads should never occur at design-time.
                if (_dispatcher == null && !IsInDesignMode)
                {

                    if (Application.Current != null)
                        _dispatcher = Application.Current.Dispatcher;
                }

                return _dispatcher;
            }
            set { _dispatcher = value; }
        }

        private static Dispatcher _dispatcher;

        /// <summary>
        /// Determines whether the calling thread is the UI thread.
        /// </summary>
        /// <returns>
        /// <see langword="true"/> if the calling thread is the UI thread; otherwise,
        /// <see langword="false"/>.
        /// </returns>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckBeginInvokeOnUI"/>
        /// <seealso cref="BeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static bool CheckAccess()
        {
            var dispatcher = Dispatcher;
            return dispatcher == null || dispatcher.CheckAccess();
        }

        /// <overloads>
        /// <summary>
        /// If necessary, executes the specified action asynchronously on the UI thread. (If the
        /// method is called on the UI thread the action is executed immediately.)
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// If necessary, executes the specified action asynchronously on the UI thread. (If the
        /// method is called on the UI thread the action is executed immediately.)
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="BeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static void CheckBeginInvokeOnUI(Action action)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;
            if (dispatcher == null || dispatcher.CheckAccess())
            {
                action();
            }
            else
            {
                dispatcher.BeginInvoke(action);
            }
        }


        /// <summary>
        /// If necessary, executes the specified action asynchronously on the UI thread. (If the
        /// method is called on the UI thread the action is executed immediately.)
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <param name="priority">
        /// The priority, relative to the other pending operations in the
        /// <see cref="System.Windows.Threading.Dispatcher"/> event queue, the specified method is
        /// invoked.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="BeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static void CheckBeginInvokeOnUI(Action action, DispatcherPriority priority)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;
            if (dispatcher == null || dispatcher.CheckAccess())
            {
                action();
            }
            else
            {
                dispatcher.BeginInvoke(action, priority);
            }
        }

        /// <summary>
        /// Executes the specified action asynchronously on the UI thread.
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="CheckBeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static void BeginInvokeOnUI(Action action)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;
            if (dispatcher == null)
            {
                action();
            }
            else
            {
                dispatcher.BeginInvoke(action);
            }
        }

        /// <overloads>
        /// <summary>
        /// Executes the specified action asynchronously on the UI thread.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Executes the specified action asynchronously on the UI thread.
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <param name="priority">
        /// The priority, relative to the other pending operations in the
        /// <see cref="System.Windows.Threading.Dispatcher"/> event queue, the specified method is
        /// invoked.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="CheckBeginInvokeOnUI"/>
        /// <seealso cref="InvokeOnUI"/>
        public static void BeginInvokeOnUI(Action action, DispatcherPriority priority)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;
            if (dispatcher == null)
            {
                action();
            }
            else
            {
                dispatcher.BeginInvoke(action, priority);
            }
        }

        /// <summary>
        /// Executes the specified action asynchronously on the UI thread.
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <returns>A task that represents the asynchronous operation.</returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        private static Task RunAsync(Action action)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;

            // Legacy implementation for .NET 4 and Silverlight 5:
            var tcs = new TaskCompletionSource<object>();
            Action wrapper = () =>
            {
                try
                {
                    action();
                    tcs.SetResult(null);
                }
                catch (Exception ex)
                {
                    tcs.SetException(ex);
                }
            };
            dispatcher.BeginInvoke(wrapper);
            return tcs.Task;
        }

        /// <summary>
        /// Executes the specified action synchronously on the UI thread. (The method blocks until
        /// the action has been executed.)
        /// </summary>
        /// <param name="action">The action that should be executed on the UI thread.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="action"/> is <see langword="null"/>.
        /// </exception>
        /// <seealso cref="Dispatcher"/>
        /// <seealso cref="CheckAccess"/>
        /// <seealso cref="CheckBeginInvokeOnUI"/>
        /// <seealso cref="BeginInvokeOnUI"/>
        public static void InvokeOnUI(Action action)
        {
            if (action == null)
                throw new ArgumentNullException(nameof(action));

            var dispatcher = Dispatcher;
            if (dispatcher == null || dispatcher.CheckAccess())
            {
                action();
            }
            else
            {
                RunAsync(action).Wait();
            }
        }

        /// <summary>
        /// Determines whether the current <see cref="DependencyObject"/> is a visual ancestor of
        /// another <see cref="DependencyObject"/>.
        /// </summary>
        /// <param name="ancestor">The dependency object expected to be the visual ancestor.</param>
        /// <param name="descendant">The dependency object expected to be the visual descendant.</param>
        /// <returns>
        /// <see langword="true"/> if <paramref name="ancestor"/> is a visual ancestor of
        /// <paramref name="descendant"/>; otherwise, <see langword="false"/>.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="descendant"/> or <paramref name="ancestor"/> is <see langword="null"/>.
        /// </exception>
        public static bool IsVisualAncestorOf(this DependencyObject ancestor, DependencyObject descendant)
        {
            return descendant.IsVisualDescendantOf(ancestor);
        }


        /// <summary>
        /// Determines whether the current <see cref="DependencyObject"/> is a visual descendant of
        /// another <see cref="DependencyObject"/>.
        /// </summary>
        /// <param name="descendant">The dependency object expected to be the visual descendant.</param>
        /// <param name="ancestor">The dependency object expected to be the visual ancestor.</param>
        /// <returns>
        /// <see langword="true"/> if <paramref name="descendant"/> is a visual descendant of
        /// <paramref name="ancestor"/>; otherwise, <see langword="false"/>.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="descendant"/> or <paramref name="ancestor"/> is <see langword="null"/>.
        /// </exception>
        public static bool IsVisualDescendantOf(this DependencyObject descendant, DependencyObject ancestor)
        {
            if (descendant == null)
                throw new ArgumentNullException(nameof(descendant));
            if (ancestor == null)
                throw new ArgumentNullException(nameof(ancestor));

            if (descendant == ancestor)
                return false;

            var parent = VisualTreeHelper.GetParent(descendant);
            while (parent != null)
            {
                if (parent == ancestor)
                    return true;

                parent = VisualTreeHelper.GetParent(parent);
            }

            return false;
        }
    }
}
