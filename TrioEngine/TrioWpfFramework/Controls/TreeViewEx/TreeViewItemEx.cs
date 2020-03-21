using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;
using TrioWpfFramework.Windows;

namespace TrioWpfFramework.Controls
{
    [TemplatePart(Name = PART_Header, Type = typeof(FrameworkElement))]
    public class TreeViewItemEx : HeaderedItemsControl
    {
        private const string PART_Header = nameof(PART_Header);
        private FrameworkElement _header;

        internal ItemsControl ParentItemsControl
        {
            get { return ItemsControlFromItemContainer(this); }
        }

        internal TreeViewItemEx ParentTreeViewItem
        {
            get { return ParentItemsControl as TreeViewItemEx; }
        }

        internal TreeViewEx ParentTreeView
        {
            get
            {
                var parent = ParentItemsControl;
                while (parent != null)
                {
                    var treeView = parent as TreeViewEx;
                    if (treeView != null)
                        return treeView;

                    parent = ItemsControlFromItemContainer(parent);
                }

                return null;
            }
        }

        internal double HeaderHeight
        {
            get { return _header?.ActualHeight ?? 0; }
        }

        internal object Item
        {
            get { return DataContext ?? this; }
        }

        private bool CanExpand
        {
            get { return HasItems; }
        }

        private static readonly DependencyPropertyKey IndentationLevelPropertyKey = DependencyProperty.RegisterReadOnly(
            "IndentationLevel",
            typeof(int),
            typeof(TreeViewItemEx),
            new FrameworkPropertyMetadata(Boxed.Int32Zero, FrameworkPropertyMetadataOptions.AffectsMeasure | FrameworkPropertyMetadataOptions.AffectsArrange));

        public static readonly DependencyProperty IndentationLevelProperty = IndentationLevelPropertyKey.DependencyProperty;

        [Browsable(false)]
        public int IndentationLevel
        {
            get { return (int)GetValue(IndentationLevelProperty); }
            //private set { SetValue(IndentationLevelPropertyKey, value); }
        }

        public static readonly DependencyProperty IsExpandedProperty = DependencyProperty.Register(
            "IsExpanded",
            typeof(bool),
            typeof(TreeViewItemEx),
            new FrameworkPropertyMetadata(
                Boxed.BooleanFalse,
                FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                OnIsExpandedChanged));

        [Description("Gets or sets the a value indicating the item is expanded and showing it children.")]
        [Category(Categories.Layout)]
        [Bindable(true)]
        public bool IsExpanded
        {
            get { return (bool)GetValue(IsExpandedProperty); }
            set { SetValue(IsExpandedProperty, Boxed.Get(value)); }
        }

        public static readonly RoutedEvent ExpandedEvent = EventManager.RegisterRoutedEvent(
            "Expanded",
            RoutingStrategy.Bubble,
            typeof(RoutedEventHandler),
            typeof(TreeViewItemEx));

        public event RoutedEventHandler Expanded
        {
            add { AddHandler(ExpandedEvent, value); }
            remove { RemoveHandler(ExpandedEvent, value); }
        }

        public static readonly RoutedEvent CollapsedEvent = EventManager.RegisterRoutedEvent(
            "Collapsed",
            RoutingStrategy.Bubble,
            typeof(RoutedEventHandler),
            typeof(TreeViewItemEx));

        public event RoutedEventHandler Collapsed
        {
            add { AddHandler(CollapsedEvent, value); }
            remove { RemoveHandler(CollapsedEvent, value); }
        }

        public static readonly DependencyProperty IsSelectedProperty = DependencyProperty.Register(
            "IsSelected",
            typeof(bool),
            typeof(TreeViewItemEx),
            new FrameworkPropertyMetadata(
                Boxed.BooleanFalse,
                FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                OnIsSelectedChanged));

        [Description("Gets or sets a value indicating whether the current TreeViewItemEx is selected in the TreeViewEx.")]
        [Category(Categories.Appearance)]
        [Bindable(true)]
        public bool IsSelected
        {
            get { return (bool)GetValue(IsSelectedProperty); }
            set { SetValue(IsSelectedProperty, Boxed.Get(value)); }
        }

        public static readonly RoutedEvent SelectedEvent = EventManager.RegisterRoutedEvent(
            "Selected",
            RoutingStrategy.Bubble,
            typeof(RoutedEventHandler),
            typeof(TreeViewItemEx));

        public event RoutedEventHandler Selected
        {
            add { AddHandler(SelectedEvent, value); }
            remove { RemoveHandler(SelectedEvent, value); }
        }

        public static readonly RoutedEvent UnselectedEvent = EventManager.RegisterRoutedEvent(
            "Unselected",
            RoutingStrategy.Bubble,
            typeof(RoutedEventHandler),
            typeof(TreeViewItemEx));

        public event RoutedEventHandler Unselected
        {
            add { AddHandler(UnselectedEvent, value); }
            remove { RemoveHandler(UnselectedEvent, value); }
        }

        static TreeViewItemEx()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(TreeViewItemEx), new FrameworkPropertyMetadata(typeof(TreeViewItemEx)));
            VirtualizingPanel.IsVirtualizingProperty.OverrideMetadata(typeof(TreeViewItemEx), new FrameworkPropertyMetadata(Boxed.BooleanFalse));
            IsTabStopProperty.OverrideMetadata(typeof(TreeViewItemEx), new FrameworkPropertyMetadata(Boxed.BooleanFalse));

            // Override BringIntoView behavior.
            EventManager.RegisterClassHandler(typeof(TreeViewItemEx), RequestBringIntoViewEvent, new RequestBringIntoViewEventHandler(OnRequestBringIntoView));
        }

        public TreeViewItemEx()
        {
            DataContextChanged += OnDataContextChanged;
        }

        private static void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs eventArgs)
        {
            var treeViewItem = (TreeViewItemEx)sender;
            var selectedItems = treeViewItem.ParentTreeView?.SelectedItems;
            if (selectedItems == null || !treeViewItem.IsSelected)
                return;

            object oldItem = eventArgs.OldValue ?? treeViewItem;
            selectedItems.Remove(oldItem);

            object newItem = eventArgs.NewValue ?? treeViewItem;
            if (!selectedItems.Contains(newItem))
                selectedItems.Add(newItem);
        }

        private static void OnIsExpandedChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var treeViewItem = (TreeViewItemEx)dependencyObject;
            bool isExpanded = (bool)eventArgs.NewValue;

            if (!isExpanded)
                treeViewItem.ParentTreeView?.HandleCollapse(treeViewItem);

            if (isExpanded)
                treeViewItem.OnExpanded(new RoutedEventArgs(ExpandedEvent, treeViewItem));
            else
                treeViewItem.OnCollapsed(new RoutedEventArgs(CollapsedEvent, treeViewItem));
        }

        private static void OnIsSelectedChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var treeViewItem = (TreeViewItemEx)dependencyObject;
            bool isSelected = (bool)eventArgs.NewValue;

            if (isSelected)
                treeViewItem.ExpandAncestors();

            // Update TreeViewEx.SelectedItems.
            var selectedItems = treeViewItem.ParentTreeView?.SelectedItems;
            if (selectedItems != null)
            {
                var item = treeViewItem.Item;
                if (isSelected)
                {
                    if (!selectedItems.Contains(item))
                        selectedItems.Add(item);
                }
                else
                {
                    selectedItems.Remove(item);
                }
            }

            if (isSelected)
                treeViewItem.OnSelected(new RoutedEventArgs(SelectedEvent, treeViewItem));
            else
                treeViewItem.OnUnselected(new RoutedEventArgs(UnselectedEvent, treeViewItem));
        }

        protected virtual void OnExpanded(RoutedEventArgs eventArgs)
        {
            if (eventArgs == null)
                throw new ArgumentNullException(nameof(eventArgs));

            Debug.Assert(eventArgs.RoutedEvent == ExpandedEvent, "Invalid arguments for TreeViewItemEx.OnExpanded.");
            RaiseEvent(eventArgs);
        }

        protected virtual void OnCollapsed(RoutedEventArgs eventArgs)
        {
            if (eventArgs == null)
                throw new ArgumentNullException(nameof(eventArgs));

            Debug.Assert(eventArgs.RoutedEvent == CollapsedEvent, "Invalid arguments for TreeViewItemEx.OnCollapsed.");
            RaiseEvent(eventArgs);
        }

        protected virtual void OnSelected(RoutedEventArgs eventArgs)
        {
            if (eventArgs == null)
                throw new ArgumentNullException(nameof(eventArgs));

            Debug.Assert(eventArgs.RoutedEvent == SelectedEvent, "Invalid arguments for TreeViewItemEx.OnSelected.");
            RaiseEvent(eventArgs);
        }

        protected virtual void OnUnselected(RoutedEventArgs eventArgs)
        {
            if (eventArgs == null)
                throw new ArgumentNullException(nameof(eventArgs));

            Debug.Assert(eventArgs.RoutedEvent == UnselectedEvent, "Invalid arguments for TreeViewItemEx.OnUnselected.");
            RaiseEvent(eventArgs);
        }

        protected override bool IsItemItsOwnContainerOverride(object item)
        {
            return item is TreeViewItemEx;
        }

        protected override DependencyObject GetContainerForItemOverride()
        {
            return new TreeViewItemEx();
        }

        protected override void PrepareContainerForItemOverride(DependencyObject element, object item)
        {
            if (element == null)
                throw new ArgumentNullException(nameof(element));

            base.PrepareContainerForItemOverride(element, item);
            element.SetValue(IndentationLevelPropertyKey, IndentationLevel + 1);
        }

        protected override void ClearContainerForItemOverride(DependencyObject element, object item)
        {
            if (element == null)
                throw new ArgumentNullException(nameof(element));

            base.ClearContainerForItemOverride(element, item);
            element.ClearValue(IndentationLevelPropertyKey);
        }

        protected override void OnItemsChanged(NotifyCollectionChangedEventArgs e)
        {
            base.OnItemsChanged(e);
            ParentTreeView?.HandleItemsChanged(this, e);
        }

        protected override void OnInitialized(EventArgs e)
        {
            base.OnInitialized(e);

            var selectedItems = ParentTreeView?.SelectedItems;
            if (selectedItems != null && selectedItems.Contains(Item))
                IsSelected = true;
        }

        public override void OnApplyTemplate()
        {
            _header = null;
            base.OnApplyTemplate();
            _header = GetTemplateChild(PART_Header) as FrameworkElement;
        }
        private static void OnRequestBringIntoView(object sender, RequestBringIntoViewEventArgs e)
        {
            if (e.TargetObject == sender)
                ((TreeViewItemEx)sender).BringIntoViewOverride(e);
        }


        private void BringIntoViewOverride(RequestBringIntoViewEventArgs e)
        {
            ExpandAncestors();

            // Only bring header into view. Ignore children.
            if (e.TargetRect.IsEmpty)
            {
                if (_header != null)
                {
                    e.Handled = true;
                    _header.BringIntoView();
                }
                else
                {
                    // Retry when loaded.
                    Dispatcher.BeginInvoke(DispatcherPriority.Loaded, new Action(() => _header?.BringIntoView()));
                }
            }
        }

        private void ExpandAncestors()
        {
            var parent = ParentTreeViewItem;
            while (parent != null)
            {
                parent.IsExpanded = true;
                parent = parent.ParentTreeViewItem;
            }
        }


        internal void FocusWithRetry()
        {
            if (!Focus())
            {
                // Delayed retry:
                Dispatcher.BeginInvoke(DispatcherPriority.Input, new Action(() => Focus()));
            }
        }
        protected override void OnGotFocus(RoutedEventArgs e)
        {
            ParentTreeView?.HandleGotFocus(this);
            base.OnGotFocus(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e == null)
                throw new ArgumentNullException(nameof(e));

            base.OnKeyDown(e);

            if (e.Handled)
                return;

            // TODO: Consider FlowDirection for Left/Right.
            switch (e.Key)
            {
                case Key.Left:
                case Key.Subtract:
                    if (CanExpand && IsEnabled && IsExpanded)
                    {
                        e.Handled = true;
                        IsExpanded = false;
                    }
                    break;

                case Key.Add:
                case Key.Right:
                    if (CanExpand && IsEnabled && !IsExpanded)
                    {
                        e.Handled = true;
                        IsExpanded = true;
                    }
                    break;
            }

            if (!e.Handled)
                ParentTreeView?.HandleKeyDown(this, e);
        }

        protected override void OnMouseDoubleClick(MouseButtonEventArgs e)
        {
            if (e == null)
                throw new ArgumentNullException(nameof(e));

            base.OnMouseDoubleClick(e);

            if (e.Handled || e.ChangedButton != MouseButton.Left || _header == null || !_header.IsMouseOver)
                return;

            if (CanExpand)
            {
                e.Handled = true;
                IsExpanded = !IsExpanded;
            }
        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            if (e == null)
                throw new ArgumentNullException(nameof(e));

            base.OnMouseDown(e);

            if (e.Handled || _header == null || !_header.IsMouseOver)
                return;

            if (e.ChangedButton == MouseButton.Left || (e.ChangedButton == MouseButton.Right && !IsSelected))
            {
                e.Handled = true;
                ParentTreeView?.MoveSelection(this);
            }
        }
    }
}
