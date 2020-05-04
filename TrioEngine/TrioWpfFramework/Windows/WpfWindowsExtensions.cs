using Estero.Linq;
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
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Threading;
using TrioWpfFramework.Mathematics;
using TrioWpfFramework.Windows.Interop;

namespace TrioWpfFramework.Windows
{
    public static class WpfWindowsExtensions
    {
        public static bool AreEqual(Point value1, Point value2)
        {
            return Numeric.AreEqual(value1.X, value2.X)
                   && Numeric.AreEqual(value1.Y, value2.Y);
        }

        public static DependencyObject GetVisualRoot(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetRoot(dependencyObject, VisualTreeHelper.GetParent);
        }

        public static IEnumerable<DependencyObject> GetVisualDescendants(this DependencyObject dependencyObject, bool depthFirst)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetDescendants(dependencyObject, GetVisualChildren, depthFirst);
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

        /// <summary>
        /// Identifies the <see cref="P:DigitalRune.Windows.WindowsHelper.SelectOnMouseDown"/>
        /// attached dependency property.
        /// </summary>
        /// 
        /// <AttachedPropertyComments>
        /// <summary>
        /// Gets or sets a value indicating whether the <strong>Selector.IsSelected</strong>
        /// attached property should be set when a mouse button is pressed over the element.
        /// </summary>
        /// <value>
        /// <see langword="true"/> to set the <strong>Selector.IsSelected</strong> attached property
        /// whenever a mouse button is pressed over the element; otherwise, <see langword="false"/>.
        /// The default value is <see langword="false"/>.
        /// </value>
        /// <remarks>
        /// This attached dependency property can be set on any <see cref="UIElement"/>. Typically
        /// it is set on <see cref="ListBoxItem"/>s. If the property is set to
        /// <see langword="true"/>, then the <strong>Selector.IsSelected</strong> attached property
        /// will be set whenever a mouse button is pressed down over the element. This is usually
        /// needed when a <see cref="ListBoxItem"/> contains an interactive element, like a
        /// <see cref="TextBox"/> because the contained element handles the mouse and the list box
        /// item is not automatically selected. Setting the
        /// <see cref="P:DigitalRune.Windows.WindowsHelper.SelectOnMouseDown"/> attached property
        /// makes sure that the list box item is properly selected.
        /// </remarks>
        /// </AttachedPropertyComments>
        public static readonly DependencyProperty SelectOnMouseDownProperty = DependencyProperty.RegisterAttached(
          "SelectOnMouseDown", typeof(bool), typeof(WpfWindowsExtensions),
          new PropertyMetadata(Boxed.BooleanFalse, OnSelectOnMouseDownChanged));


        /// <summary>
        /// Gets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.SelectOnMouseDown"/>
        /// attached property from a given <see cref="DependencyObject"/> object.
        /// </summary>
        /// <param name="obj">The object from which to read the property value.</param>
        /// <returns>
        /// The value of the <see cref="P:DigitalRune.Windows.WindowsHelper.SelectOnMouseDown"/>
        /// attached property.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="obj"/> is <see langword="null"/>.
        /// </exception>
        public static bool GetSelectOnMouseDown(DependencyObject obj)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            return (bool)obj.GetValue(SelectOnMouseDownProperty);
        }

        /// <summary>
        /// Sets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.SelectOnMouseDown"/>
        /// attached property to a given <see cref="DependencyObject"/> object.
        /// </summary>
        /// <param name="obj">The object on which to set the property value.</param>
        /// <param name="value">The property value to set.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="obj"/> is <see langword="null"/>.
        /// </exception>
        public static void SetSelectOnMouseDown(DependencyObject obj, bool value)
        {
            if (obj == null)
                throw new ArgumentNullException(nameof(obj));

            obj.SetValue(SelectOnMouseDownProperty, Boxed.Get(value));
        }


        private static void OnSelectOnMouseDownChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var element = dependencyObject as UIElement;
            if (element == null)
                return;

            if ((bool)eventArgs.NewValue)
            {
                element.PreviewMouseDown += OnPreviewMouseDown;
            }
            else
            {
                element.PreviewMouseDown -= OnPreviewMouseDown;
            }
        }


        private static void OnPreviewMouseDown(object sender, MouseButtonEventArgs eventArgs)
        {
            var element = sender as UIElement;
            if (element == null)
                return;

            Selector.SetIsSelected(element, true);
        }

        /// <summary>
        /// Gets the visual parent of the <see cref="DependencyObject"/>.
        /// </summary>
        /// <param name="dependencyObject">
        /// The <see cref="DependencyObject"/> to get the visual parent for.
        /// </param>
        /// <returns>
        /// The visual parent of the <see cref="DependencyObject"/>.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="dependencyObject"/> is <see langword="null"/>.
        /// </exception>
        public static DependencyObject GetVisualParent(this DependencyObject dependencyObject)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return VisualTreeHelper.GetParent(dependencyObject);
        }


        /// <summary>
        /// Identifies the <see cref="P:DigitalRune.Windows.WindowsHelper.ShowIcon"/> attached
        /// dependency property.
        /// </summary>
        /// <AttachedPropertyComments>
        /// <summary>
        /// Gets or sets a value indicating whether the window icon is visible in the caption bar.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the window icon is visible; otherwise,
        /// <see langword="false"/>. The default value is <see langword="true"/>.
        /// </value>
        /// </AttachedPropertyComments>
        public static readonly DependencyProperty ShowIconProperty = DependencyProperty.RegisterAttached(
            "ShowIcon",
            typeof(bool),
            typeof(WpfWindowsExtensions),
            new PropertyMetadata(Boxed.BooleanTrue, OnShowIconChanged));

        /// <summary>
        /// Gets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.ShowIcon"/>
        /// attached property from a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object from which to read the property value.</param>
        /// <returns>
        /// The value of the <see cref="P:DigitalRune.Windows.WindowsHelper.ShowIcon"/> attached
        /// property.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static bool GetShowIcon(DependencyObject window)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            return (bool)window.GetValue(ShowIconProperty);
        }

        /// <summary>
        /// Sets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.ShowIcon"/>
        /// attached property to a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object on which to set the property value.</param>
        /// <param name="value">The property value to set.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static void SetShowIcon(DependencyObject window, bool value)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            window.SetValue(ShowIconProperty, Boxed.Get(value));
        }


        /// <summary>
        /// Identifies the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMinimize"/> attached
        /// dependency property.
        /// </summary>
        /// <AttachedPropertyComments>
        /// <summary>
        /// Gets or sets a value indicating whether the window has a minimize button.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the window has a minimize button; otherwise,
        /// <see langword="false"/>. The default value is <see langword="true"/>.
        /// </value>
        /// </AttachedPropertyComments>
        public static readonly DependencyProperty CanMinimizeProperty = DependencyProperty.RegisterAttached(
            "CanMinimize",
            typeof(bool),
            typeof(WpfWindowsExtensions),
            new PropertyMetadata(Boxed.BooleanTrue, OnCanMinimizeChanged));

        /// <summary>
        /// Gets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMinimize"/>
        /// attached property from a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object from which to read the property value.</param>
        /// <returns>
        /// The value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMinimize"/> attached
        /// property.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static bool GetCanMinimize(DependencyObject window)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            return (bool)window.GetValue(CanMinimizeProperty);
        }

        /// <summary>
        /// Sets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMinimize"/>
        /// attached property to a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object on which to set the property value.</param>
        /// <param name="value">The property value to set.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static void SetCanMinimize(DependencyObject window, bool value)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            window.SetValue(CanMinimizeProperty, Boxed.Get(value));
        }


        /// <summary>
        /// Identifies the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMaximize"/> attached
        /// dependency property.
        /// </summary>
        /// <AttachedPropertyComments>
        /// <summary>
        /// Gets or sets a value indicating whether the window has a maximize button.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the window has a maximize button; otherwise,
        /// <see langword="false"/>. The default value is <see langword="true"/>.
        /// </value>
        /// </AttachedPropertyComments>
        public static readonly DependencyProperty CanMaximizeProperty = DependencyProperty.RegisterAttached(
          "CanMaximize",
          typeof(bool),
          typeof(WpfWindowsExtensions),
          new PropertyMetadata(Boxed.BooleanTrue, OnCanMaximizeChanged));

        /// <summary>
        /// Gets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMaximize"/>
        /// attached property from a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object from which to read the property value.</param>
        /// <returns>
        /// The value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMaximize"/> attached
        /// property.
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static bool GetCanMaximize(DependencyObject window)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            return (bool)window.GetValue(CanMaximizeProperty);
        }

        /// <summary>
        /// Sets the value of the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMaximize"/>
        /// attached property to a given <see cref="Window"/> object.
        /// </summary>
        /// <param name="window">The object on which to set the property value.</param>
        /// <param name="value">The property value to set.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="window"/> is <see langword="null"/>.
        /// </exception>
        public static void SetCanMaximize(DependencyObject window, bool value)
        {
            if (window == null)
                throw new ArgumentNullException(nameof(window));

            window.SetValue(CanMaximizeProperty, Boxed.Get(value));
        }


        /// <summary>
        /// Called when the <see cref="P:DigitalRune.Windows.WindowsHelper.ShowIcon"/> property
        /// changed.
        /// </summary>
        /// <param name="dependencyObject">The dependency object.</param>
        /// <param name="eventArgs">
        /// The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.
        /// </param>
        private static void OnShowIconChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            bool newValue = (bool)eventArgs.NewValue;
            var window = dependencyObject as Window;
            if (window == null)
                return;

            if (window.IsLoaded)
            {
                if (newValue)
                    ShowWindowIcon(window);
                else
                    HideWindowIcon(window);
            }
            else
            {
                EventHandler handler = null;
                handler = (s, e) =>
                {
                    window.SourceInitialized -= handler;

                    if (newValue)
                        ShowWindowIcon(window);
                    else
                        HideWindowIcon(window);
                };
                window.SourceInitialized += handler;
            }
        }


        /// <summary>
        /// Called when the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMinimize"/> property
        /// changed.
        /// </summary>
        /// <param name="dependencyObject">The dependency object.</param>
        /// <param name="eventArgs">
        /// The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.
        /// </param>
        private static void OnCanMinimizeChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            bool newValue = (bool)eventArgs.NewValue;
            var window = dependencyObject as Window;
            if (window == null)
                return;

            if (window.IsLoaded)
            {
                if (newValue)
                    ShowMinimizeBox(window);
                else
                    HideMinimizeBox(window);
            }
            else
            {
                EventHandler handler = null;
                handler = (s, e) =>
                {
                    window.SourceInitialized -= handler;

                    if (newValue)
                        ShowMinimizeBox(window);
                    else
                        HideMinimizeBox(window);
                };
                window.SourceInitialized += handler;
            }
        }

        /// <summary>
        /// Gets the visual subtree (the given <see cref="DependencyObject"/> and all of its
        /// descendants) using either a depth- or a breadth-first search.
        /// </summary>
        /// <param name="dependencyObject">
        /// The <see cref="DependencyObject"/> to get the visual descendants for.
        /// </param>
        /// <param name="depthFirst">
        /// If set to <see langword="true"/> then a depth-first search for descendants will be made;
        /// otherwise a breadth-first search will be made.
        /// </param>
        /// <returns>The visual descendants of the <see cref="DependencyObject"/>.</returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="dependencyObject"/> is <see langword="null"/>.
        /// </exception>
        public static IEnumerable<DependencyObject> GetVisualSubtree(this DependencyObject dependencyObject, bool depthFirst)
        {
            if (dependencyObject == null)
                throw new ArgumentNullException(nameof(dependencyObject));

            return TreeHelper.GetSubtree(dependencyObject, GetVisualChildren, depthFirst);
        }


        /// <summary>
        /// Called when the <see cref="P:DigitalRune.Windows.WindowsHelper.CanMaximize"/> property
        /// changed.
        /// </summary>
        /// <param name="dependencyObject">The dependency object.</param>
        /// <param name="eventArgs">
        /// The <see cref="DependencyPropertyChangedEventArgs"/> instance containing the event data.
        /// </param>
        private static void OnCanMaximizeChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            bool newValue = (bool)eventArgs.NewValue;
            var window = dependencyObject as Window;
            if (window == null)
                return;

            if (window.IsLoaded)
            {
                if (newValue)
                    ShowMaximizeBox(window);
                else
                    HideMaximizeBox(window);
            }
            else
            {
                EventHandler handler = null;
                handler = (s, e) =>
                {
                    window.SourceInitialized -= handler;

                    if (newValue)
                        ShowMaximizeBox(window);
                    else
                        HideMaximizeBox(window);
                };
                window.SourceInitialized += handler;
            }
        }


        private static void ShowWindowIcon(Window window)
        {
            // Not implemented.
        }


        private static void HideWindowIcon(Window window)
        {
            IntPtr hwnd = new WindowInteropHelper(window).Handle;
            int value = Win32.GetWindowLong(hwnd, GetWindowLongIndex.GWL_EXSTYLE);
            Win32.SetWindowLong(hwnd, GetWindowLongIndex.GWL_EXSTYLE, (int)(value | WindowStylesEx.WS_EX_DLGMODALFRAME));

            // Update the window's non-client area to reflect the changes.
            //Win32.SetWindowPos(hwnd, IntPtr.Zero, 0, 0, 0, 0, SetWindowPosFlags.SWP_NOMOVE | SetWindowPosFlags.SWP_NOSIZE | SetWindowPosFlags.SWP_NOZORDER | SetWindowPosFlags.SWP_FRAMECHANGED);

            // Fix for dialog windows. See http://stackoverflow.com/questions/18580430/hide-the-icon-from-a-wpf-window.
            Win32.SendMessage(hwnd, WindowMessages.WM_SETICON, new IntPtr(1), IntPtr.Zero);
            Win32.SendMessage(hwnd, WindowMessages.WM_SETICON, IntPtr.Zero, IntPtr.Zero);
        }
        
        private static void ShowMinimizeBox(Window window)
        {
            IntPtr hwnd = new WindowInteropHelper(window).Handle;
            int value = Win32.GetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE);
            Win32.SetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE, (int)(value | WindowStyles.WS_MINIMIZEBOX));
        }


       private static void HideMinimizeBox(Window window)
        {
            IntPtr hwnd = new WindowInteropHelper(window).Handle;
            int value = Win32.GetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE);
            Win32.SetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE, (int)(value & ~WindowStyles.WS_MINIMIZEBOX));
        }


        private static void ShowMaximizeBox(Window window)
        {
            IntPtr hwnd = new WindowInteropHelper(window).Handle;
            int value = Win32.GetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE);
            Win32.SetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE, (int)(value | WindowStyles.WS_MAXIMIZEBOX));
        }


        private static void HideMaximizeBox(Window window)
        {
            IntPtr hwnd = new WindowInteropHelper(window).Handle;
            int value = Win32.GetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE);
            Win32.SetWindowLong(hwnd, GetWindowLongIndex.GWL_STYLE, (int)(value & ~WindowStyles.WS_MAXIMIZEBOX));
        }

    }
}
