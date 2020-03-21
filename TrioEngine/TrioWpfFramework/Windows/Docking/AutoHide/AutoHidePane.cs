﻿
using System;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Threading;
using TrioWpfFramework.Mathematics;
using TrioWpfFramework.Windows.Docking.Controls;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.Interop;

using static System.FormattableString;

namespace TrioWpfFramework.Windows.Docking
{
    [TemplatePart(Name = "PART_Grid", Type = typeof(Grid))]
    [TemplatePart(Name = "PART_ContentPresenter", Type = typeof(ContentPresenter))]
    [TemplatePart(Name = "PART_ResizeThumb", Type = typeof(Thumb))]
    [TemplateVisualState(GroupName = "DockStates", Name = "Left")]
    [TemplateVisualState(GroupName = "DockStates", Name = "Right")]
    [TemplateVisualState(GroupName = "DockStates", Name = "Top")]
    [TemplateVisualState(GroupName = "DockStates", Name = "Bottom")]
    public class AutoHidePane : ContentControl
    {
        private const double AdditionalOffset = 10;

        // Template parts
        private Grid _grid;
        private ContentPresenter _contentPresenter;
        private Thumb _resizeThumb;

        // Animation properties
        private readonly DispatcherTimer _timer;
        private readonly TranslateTransform _translateTransform;
        private Storyboard _slideInAnimation;
        private Storyboard _slideOutAnimation;

        // Focus handling
        private bool _isFocusDeferred; // True if the auto-hide pane receives focus as soon as it is loaded.

        public DockControl DockControl { get; private set; }

        private bool UseAnimations
        {
            get
            {
                // Disable animations if pane contains Win32 or WinForm controls.
                return !this.GetVisualDescendants().OfType<HwndHost>().Any();
            }
        }

        public event EventHandler<EventArgs> Shown;

        public event EventHandler<EventArgs> Hidden;

        public static readonly DependencyProperty DockProperty = DockPanel.DockProperty.AddOwner(
            typeof(AutoHidePane),
            new FrameworkPropertyMetadata(Dock.Right, OnDockPropertyChanged));

        public Dock Dock
        {
            get { return (Dock)GetValue(DockProperty); }
            set { SetValue(DockProperty, value); }
        }

        public static readonly DependencyProperty HideAutomaticallyProperty = DependencyProperty.Register(
           "HideAutomatically",
           typeof(bool),
           typeof(AutoHidePane),
           new FrameworkPropertyMetadata(Boxed.BooleanTrue, OnHideAutomaticallyChanged));

        public bool HideAutomatically
        {
            get { return (bool)GetValue(HideAutomaticallyProperty); }
            set { SetValue(HideAutomaticallyProperty, Boxed.Get(value)); }
        }

        public static readonly DependencyProperty SlideDurationProperty = DependencyProperty.Register(
            "SlideDuration",
            typeof(TimeSpan),
            typeof(AutoHidePane),
            new FrameworkPropertyMetadata(TimeSpan.FromSeconds(0.25)));

        public TimeSpan SlideDuration
        {
            get { return (TimeSpan)GetValue(SlideDurationProperty); }
            set { SetValue(SlideDurationProperty, value); }
        }

        private static readonly DependencyPropertyKey StatePropertyKey = DependencyProperty.RegisterReadOnly(
            "State",
            typeof(AutoHideState),
            typeof(AutoHidePane),
            new FrameworkPropertyMetadata(AutoHideState.Hidden));

        public static readonly DependencyProperty StateProperty = StatePropertyKey.DependencyProperty;

        public AutoHideState State
        {
            get { return (AutoHideState)GetValue(StateProperty); }
            private set { SetValue(StatePropertyKey, value); }
        }

        public static readonly DependencyProperty TimeoutProperty = DependencyProperty.Register(
            "Timeout",
            typeof(TimeSpan),
            typeof(AutoHidePane),
            new FrameworkPropertyMetadata(TimeSpan.FromSeconds(1), OnTimeoutChanged));

        public TimeSpan Timeout
        {
            get { return (TimeSpan)GetValue(TimeoutProperty); }
            set { SetValue(TimeoutProperty, value); }
        }

        static AutoHidePane()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(AutoHidePane), new FrameworkPropertyMetadata(typeof(AutoHidePane)));
        }

        public AutoHidePane()
        {
            // Use a timer to measure timeout.
            _timer = new DispatcherTimer { Interval = Timeout };
            _timer.Tick += OnTimeout;

            // Setup a render transform which is animated to slide the pane in and out.
            _translateTransform = new TranslateTransform { X = 0, Y = 0 };
            RenderTransform = _translateTransform;

            Loaded += OnLoaded;
        }

        protected override void OnContentChanged(object oldContent, object newContent)
        {
            base.OnContentChanged(oldContent, newContent);
            UpdateAutoHideSize();
        }

        private static void OnDockPropertyChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var autoHidePane = (AutoHidePane)dependencyObject;
            autoHidePane.OnDockPropertyChanged();
        }

        private void OnDockPropertyChanged()
        {
            StopAutomaticTimeout();
            ResetAnimations();

            switch (Dock)
            {
                case Dock.Left:
                    HorizontalAlignment = HorizontalAlignment.Left;
                    VerticalAlignment = VerticalAlignment.Stretch;
                    if (_resizeThumb != null)
                    {
                        Grid.SetColumn(_resizeThumb, 2);
                        Grid.SetRow(_resizeThumb, 1);
                        _resizeThumb.Width = 4;
                        _resizeThumb.Height = double.NaN;
                        _resizeThumb.Cursor = Cursors.SizeWE;
                    }
                    break;
                case Dock.Right:
                    HorizontalAlignment = HorizontalAlignment.Right;
                    VerticalAlignment = VerticalAlignment.Stretch;
                    if (_resizeThumb != null)
                    {
                        Grid.SetColumn(_resizeThumb, 0);
                        Grid.SetRow(_resizeThumb, 1);
                        _resizeThumb.Width = 4;
                        _resizeThumb.Height = double.NaN;
                        _resizeThumb.Cursor = Cursors.SizeWE;
                    }
                    break;
                case Dock.Top:
                    HorizontalAlignment = HorizontalAlignment.Stretch;
                    VerticalAlignment = VerticalAlignment.Top;
                    if (_resizeThumb != null)
                    {
                        Grid.SetColumn(_resizeThumb, 1);
                        Grid.SetRow(_resizeThumb, 2);
                        _resizeThumb.Width = double.NaN;
                        _resizeThumb.Height = 4;
                        _resizeThumb.Cursor = Cursors.SizeNS;
                    }
                    break;
                case Dock.Bottom:
                    HorizontalAlignment = HorizontalAlignment.Stretch;
                    VerticalAlignment = VerticalAlignment.Bottom;
                    if (_resizeThumb != null)
                    {
                        Grid.SetColumn(_resizeThumb, 1);
                        Grid.SetRow(_resizeThumb, 0);
                        _resizeThumb.Width = double.NaN;
                        _resizeThumb.Height = 4;
                        _resizeThumb.Cursor = Cursors.SizeNS;
                    }
                    break;
            }

            UpdateAutoHideSize();
            UpdateVisualStates(true);
        }


        private static void OnHideAutomaticallyChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var autoHidePane = (AutoHidePane)dependencyObject;
            autoHidePane.OnHideAutomaticallyChanged((bool)eventArgs.NewValue);
        }


        private void OnHideAutomaticallyChanged(bool newValue)
        {
            if (newValue)
                StartAutomaticTimeout();
            else
                StopAutomaticTimeout();
        }


        private static void OnTimeoutChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var autoHidePane = (AutoHidePane)dependencyObject;
            autoHidePane.OnTimeoutChanged((TimeSpan)eventArgs.NewValue);
        }


        private void OnTimeoutChanged(TimeSpan newValue)
        {
            _timer.Interval = newValue;
        }

        public override void OnApplyTemplate()
        {
            _grid = null;
            _contentPresenter = null;
            if (_resizeThumb != null)
            {
                _resizeThumb.DragDelta -= OnResizeThumbDragged;
                _resizeThumb.MouseDoubleClick -= OnResizeThumbDoubleClicked;
                _resizeThumb = null;
            }

            base.OnApplyTemplate();

            _grid = GetTemplateChild("PART_Grid") as Grid;
            _contentPresenter = GetTemplateChild("PART_ContentPresenter") as ContentPresenter;
            _resizeThumb = GetTemplateChild("PART_ResizeThumb") as Thumb;

            if (_grid != null)
            {
                _grid.ColumnDefinitions.Clear();
                _grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Auto) });
                _grid.ColumnDefinitions.Add(new ColumnDefinition());
                _grid.ColumnDefinitions.Add(new ColumnDefinition { Width = new GridLength(1, GridUnitType.Auto) });

                _grid.RowDefinitions.Clear();
                _grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Auto) });
                _grid.RowDefinitions.Add(new RowDefinition());
                _grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Auto) });
            }

            if (_contentPresenter != null)
            {
                // Place content in center of grid.
                Grid.SetColumn(_contentPresenter, 1);
                Grid.SetRow(_contentPresenter, 1);
            }

            if (_resizeThumb != null)
            {
                _resizeThumb.DragDelta += OnResizeThumbDragged;
                _resizeThumb.MouseDoubleClick += OnResizeThumbDoubleClicked;
            }

            OnDockPropertyChanged();
            UpdateVisualStates(false);
        }

        private void UpdateVisualStates(bool useTransitions)
        {
            switch (Dock)
            {
                case Dock.Left:
                    VisualStateManager.GoToState(this, "Left", useTransitions);
                    break;
                case Dock.Right:
                    VisualStateManager.GoToState(this, "Right", useTransitions);
                    break;
                case Dock.Top:
                    VisualStateManager.GoToState(this, "Top", useTransitions);
                    break;
                case Dock.Bottom:
                    VisualStateManager.GoToState(this, "Bottom", useTransitions);
                    break;
            }
        }


        private void OnLoaded(object sender, RoutedEventArgs eventArgs)
        {
            DockControl = DockHelper.GetDockControl(this);
            UpdateAutoHideSize();
            StopAutomaticTimeout();
            ResetAnimations();
        }

        private void UpdateAutoHideSize()
        {
            var dockTabPane = Content as IDockTabPane;
            var dockTabItem = dockTabPane?.SelectedItem;
            if (dockTabItem == null)
            {
                Width = double.NaN;
                Height = double.NaN;
                return;
            }

            // Copy dockTabItem.AutoHideWidth/Height to this.AutoHideWidth/Height.
            // When a dockTabItem with *-sized dock width/height is used and the AutoHideWidth/Height
            // is NaN, the window might fill out the entire DockControl area. In this case, we set
            // 1/3 of the DockControl area as the initial size.

            var dock = Dock;
            if (dock == Dock.Left || dock == Dock.Right)
            {
                double width = dockTabItem.AutoHideWidth;
                if (DockControl != null && dockTabItem.DockWidth.GridUnitType == GridUnitType.Star && Numeric.IsNaN(width))
                    width = DockControl.ActualWidth / 3;

                Width = width;
                Height = double.NaN;
            }
            else
            {
                double height = dockTabItem.AutoHideHeight;
                if (DockControl != null && dockTabItem.DockWidth.GridUnitType == GridUnitType.Star && Numeric.IsNaN(height))
                    height = DockControl.ActualHeight / 3;

                Width = double.NaN;
                Height = height;
            }
        }


        private void SaveAutoHideSize()
        {
            var dockTabPane = Content as IDockTabPane;
            if (dockTabPane != null)
            {
                var dock = Dock;
                if (dock == Dock.Left || dock == Dock.Right)
                {
                    double autoHideWidth = Width;
                    foreach (var item in dockTabPane.Items)
                        item.AutoHideWidth = autoHideWidth;
                }
                else
                {
                    double autoHideHeight = Height;
                    foreach (var item in dockTabPane.Items)
                        item.AutoHideHeight = autoHideHeight;
                }
            }
        }


        private void OnResizeThumbDragged(object sender, DragDeltaEventArgs eventArgs)
        {
            double autoHideWidth = ActualWidth;
            double autoHideHeight = ActualHeight;

            switch (Dock)
            {
                case Dock.Left:
                    autoHideWidth += eventArgs.HorizontalChange;
                    break;
                case Dock.Right:
                    autoHideWidth -= eventArgs.HorizontalChange;
                    break;
                case Dock.Top:
                    autoHideHeight += eventArgs.VerticalChange;
                    break;
                case Dock.Bottom:
                    autoHideHeight -= eventArgs.VerticalChange;
                    break;
            }

            double minWidth = MinWidth;
            double minHeight = MinHeight;
            double maxWidth = MaxWidth;
            double maxHeight = MaxHeight;

            var panel = this.GetLogicalAncestors().OfType<Panel>().FirstOrDefault();
            if (panel != null)
            {
                maxWidth = Math.Min(maxWidth, panel.ActualWidth);
                maxHeight = Math.Min(maxHeight, panel.ActualHeight);
            }

            if (autoHideWidth < minWidth)
                autoHideWidth = minWidth;
            else if (autoHideWidth > maxWidth)
                autoHideWidth = maxWidth;


            if (autoHideHeight < minHeight)
                autoHideHeight = minHeight;
            else if (autoHideHeight > maxHeight)
                autoHideHeight = maxHeight;

            switch (Dock)
            {
                case Dock.Left:
                case Dock.Right:
                    Width = autoHideWidth;
                    break;
                case Dock.Top:
                case Dock.Bottom:
                    Height = autoHideHeight;
                    break;
            }

            SaveAutoHideSize();
        }


        private void OnResizeThumbDoubleClicked(object sender, MouseButtonEventArgs eventArgs)
        {
            Width = double.NaN;
            Height = double.NaN;
            SaveAutoHideSize();
        }

        private void ResetAnimations()
        {
            if (!UseAnimations)
                return;

            RemoveSlideInAnimation();
            RemoveSlideOutAnimation();

            double offset = GetOffScreenOffset();
            switch (Dock)
            {
                case Dock.Left:
                case Dock.Right:
                    _translateTransform.X = offset;
                    _translateTransform.Y = 0;
                    break;
                case Dock.Top:
                case Dock.Bottom:
                    _translateTransform.X = 0;
                    _translateTransform.Y = offset;
                    break;
                default:
                    throw new NotSupportedException(Invariant($"Invalid value for AutoHidePane.Dock: {Dock}"));
            }

            Visibility = Visibility.Collapsed;
            State = AutoHideState.Hidden;
        }

        public void Show()
        {
            // Bring AutoHideOverlay to front.
            var autoHideOverlay = Window.GetWindow(this);
            if (autoHideOverlay != null)
            {
                var hWnd = new WindowInteropHelper(autoHideOverlay).Handle;
                Win32.SetWindowPos(hWnd, IntPtr.Zero, 0, 0, 0, 0, SetWindowPosFlags.SWP_NOMOVE | SetWindowPosFlags.SWP_NOSIZE | SetWindowPosFlags.SWP_SHOWWINDOW | SetWindowPosFlags.SWP_NOACTIVATE);

                // Alternative:
                //autoHideOverlay.Topmost = true;
                //autoHideOverlay.Topmost = false;
            }

            if (!IsLoaded)
            {
                Loaded += DeferredShow;
                return;
            }

            StopAutomaticTimeout();

            if (State == AutoHideState.SlidingIn || State == AutoHideState.Shown)
                return;

            if (UseAnimations)
            {
                RemoveSlideOutAnimation();

                Visibility = Visibility.Visible;
                State = AutoHideState.SlidingIn;

                _slideInAnimation = new Storyboard();
                var animation = new DoubleAnimation
                {
                    To = 0,
                    Duration = new Duration(SlideDuration),
                    EasingFunction = new CubicEase(),
                };
                Storyboard.SetTarget(animation, this);
                Storyboard.SetTargetProperty(animation, GetAnimatedProperty());
                _slideInAnimation.Children.Add(animation);
                _slideInAnimation.Completed += OnSlideInAnimationCompleted;
                _slideInAnimation.Begin();
            }
            else
            {
                Visibility = Visibility.Visible;
                OnSlideInAnimationCompleted(this, EventArgs.Empty);
            }
        }


        private void DeferredShow(object sender, RoutedEventArgs eventArgs)
        {
            Loaded -= DeferredShow;
            Show();
        }


        /// <summary>
        /// Slides the <see cref="AutoHidePane"/> out of the view.
        /// </summary>
        public void Hide()
        {
            if (!IsLoaded)
            {
                Loaded += DeferredHide;
                return;
            }

            StopAutomaticTimeout();

            if (State == AutoHideState.SlidingOut || State == AutoHideState.Hidden)
                return;

            if (UseAnimations)
            {
                RemoveSlideInAnimation();

                State = AutoHideState.SlidingOut;

                _slideOutAnimation = new Storyboard();
                var animation = new DoubleAnimation
                {
                    To = GetOffScreenOffset(),
                    Duration = new Duration(SlideDuration),
                    EasingFunction = new CubicEase(),
                };
                Storyboard.SetTarget(animation, this);
                Storyboard.SetTargetProperty(animation, GetAnimatedProperty());
                _slideOutAnimation.Children.Add(animation);
                _slideOutAnimation.Completed += OnSlideOutAnimationCompleted;
                _slideOutAnimation.Begin();
            }
            else
            {
                OnSlideOutAnimationCompleted(this, EventArgs.Empty);
            }
        }


        private void DeferredHide(object sender, RoutedEventArgs eventArgs)
        {
            Loaded -= DeferredHide;
            Hide();
        }


        private void RemoveSlideInAnimation()
        {
            if (_slideInAnimation == null)
                return;

            _slideInAnimation.Completed -= OnSlideInAnimationCompleted;
            _slideInAnimation.Stop();
            _slideInAnimation.Remove();
            _slideInAnimation = null;
        }


        private void RemoveSlideOutAnimation()
        {
            if (_slideOutAnimation == null)
                return;

            _slideOutAnimation.Completed -= OnSlideOutAnimationCompleted;
            _slideOutAnimation.Stop();
            _slideOutAnimation.Remove();
            _slideOutAnimation = null;
        }


        private double GetOffScreenOffset()
        {
            switch (Dock)
            {
                case Dock.Left:
                    return -(ActualWidth + AdditionalOffset);
                case Dock.Right:
                    return ActualWidth + AdditionalOffset;
                case Dock.Top:
                    return -(ActualHeight + AdditionalOffset);
                case Dock.Bottom:
                    return ActualHeight + AdditionalOffset;
                default:
                    throw new NotSupportedException(Invariant($"Invalid value for AutoHidePane.Dock: {Dock}"));
            }
        }


        private PropertyPath GetAnimatedProperty()
        {
            switch (Dock)
            {
                case Dock.Left:
                case Dock.Right:
                    return new PropertyPath("(UIElement.RenderTransform).(TranslateTransform.X)");
                case Dock.Top:
                case Dock.Bottom:
                    return new PropertyPath("(UIElement.RenderTransform).(TranslateTransform.Y)");
                default:
                    throw new NotSupportedException(Invariant($"Invalid value for AutoHidePane.Dock: {Dock}"));
            }
        }


        private void OnSlideInAnimationCompleted(object sender, EventArgs eventArgs)
        {
            State = AutoHideState.Shown;
            StartAutomaticTimeout();
            OnShown(EventArgs.Empty);
        }


        private void OnSlideOutAnimationCompleted(object sender, EventArgs eventArgs)
        {
            Visibility = Visibility.Collapsed;
            State = AutoHideState.Hidden;
            StopAutomaticTimeout();
            OnHidden(EventArgs.Empty);
        }


        /// <summary>
        /// Raises the <see cref="Shown"/> event.
        /// </summary>
        /// <param name="eventArgs">
        /// <see cref="EventArgs"/> object that provides the arguments for the event.
        /// </param>
        /// <remarks>
        /// <strong>Notes to Inheritors: </strong>When overriding <see cref="OnShown"/> in a 
        /// derived class, be sure to call the base class's <see cref="OnShown"/> method so that 
        /// registered delegates receive the event.
        /// </remarks>
        protected virtual void OnShown(EventArgs eventArgs)
        {
            Shown?.Invoke(this, eventArgs);
        }


        /// <summary>
        /// Raises the <see cref="Hidden"/> event.
        /// </summary>
        /// <param name="eventArgs">
        /// <see cref="EventArgs"/> object that provides the arguments for the event.
        /// </param>
        /// <remarks>
        /// <strong>Notes to Inheritors: </strong>When overriding <see cref="OnHidden"/> in a 
        /// derived class, be sure to call the base class's <see cref="OnHidden"/> method so that 
        /// registered delegates receive the event.
        /// </remarks>
        protected virtual void OnHidden(EventArgs eventArgs)
        {
            Hidden?.Invoke(this, eventArgs);
        }

        public bool FocusContent()
        {
            if (!IsLoaded)
            {
                Loaded += DeferredFocusContent;
                _isFocusDeferred = true;
                return false;
            }

            bool hasFocusMoved = false;

            var dockTabPane = _contentPresenter.GetContentContainer<DockTabPane>();
            if (dockTabPane != null)
                hasFocusMoved = dockTabPane.FocusSelectedContent();
            else if (_contentPresenter != null)
                hasFocusMoved = _contentPresenter.MoveFocus(new TraversalRequest(FocusNavigationDirection.First));

            return hasFocusMoved;
        }


        private void DeferredFocusContent(object sender, RoutedEventArgs eventArgs)
        {
            Loaded -= DeferredFocusContent;
            _isFocusDeferred = false;
            FocusContent();
        }


        /// <summary>
        /// Invoked when an unhandled <strong>Keyboard.GotKeyboardFocus</strong> attached event reaches 
        /// an element in its route that is derived from this class. Implement this method to add class 
        /// handling for this event.
        /// </summary>
        /// <param name="e">
        /// The <see cref="KeyboardFocusChangedEventArgs"/> that contains the event data.
        /// </param>
        protected override void OnGotKeyboardFocus(KeyboardFocusChangedEventArgs e)
        {
            base.OnGotKeyboardFocus(e);
            Show();
        }


        /// <summary>
        /// Invoked when an unhandled <see cref="UIElement.MouseLeftButtonDown"/> routed event is raised 
        /// on this element. Implement this method to add class handling for this event.
        /// </summary>
        /// <param name="e">
        /// The <see cref="MouseButtonEventArgs"/> that contains the event data. The event data reports 
        /// that the left mouse button was pressed.
        /// </param>
        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);

            if (!e.Handled)
                FocusContent();
        }


        /// <summary>
        /// Invoked when an unhandled <strong>Mouse.MouseEnter</strong> attached event is raised on this 
        /// element. Implement this method to add class handling for this event.
        /// </summary>
        /// <param name="e">
        /// The <see cref="MouseEventArgs"/> that contains the event data.
        /// </param>
        protected override void OnMouseEnter(MouseEventArgs e)
        {
            base.OnMouseEnter(e);
            Show();
        }


        /// <summary>
        /// Invoked when an unhandled <strong>Mouse.MouseLeave</strong> attached event is raised on this 
        /// element. Implement this method to add class handling for this event.
        /// </summary>
        /// <param name="e">
        /// The <see cref="MouseEventArgs"/> that contains the event data.
        /// </param>
        protected override void OnMouseLeave(MouseEventArgs e)
        {
            base.OnMouseLeave(e);
            StartAutomaticTimeout();
        }


        private void StartAutomaticTimeout()
        {
            if (HideAutomatically && !IsKeyboardFocusWithin && !_isFocusDeferred && !IsMouseOver)
                _timer.Start();
        }


        private void StopAutomaticTimeout()
        {
            _timer.Stop();
        }


        private void OnTimeout(object sender, EventArgs eventArgs)
        {
            _timer.Stop();
            if (!IsKeyboardFocusWithin && !IsMouseOver)
                Hide();
        }
    }
}
