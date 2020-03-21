using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;

namespace TrioWpfFramework.Windows.Docking
{
    [TemplateVisualState(GroupName = "CommonStates", Name = "Normal")]
    [TemplateVisualState(GroupName = "CommonStates", Name = "MouseOver")]
    [TemplateVisualState(GroupName = "CommonStates", Name = "Disabled")]
    [TemplateVisualState(GroupName = "LayoutStates", Name = "Upright")]
    [TemplateVisualState(GroupName = "LayoutStates", Name = "UpsideDown")]
    public class AutoHideTab : Control
    {
        private readonly DispatcherTimer _hoverTimer;

        internal AutoHideGroup AutoHideGroup
        {
            get { return ItemsControl.ItemsControlFromItemContainer(this) as AutoHideGroup; }
        }

        internal AutoHideBar AutoHideBar
        {
            get
            {
                var autoHideGroup = AutoHideGroup;
                if (autoHideGroup == null)
                    return null;

                return ItemsControl.ItemsControlFromItemContainer(autoHideGroup) as AutoHideBar;
            }
        }

        private bool IsUpsideDown
        {
            get
            {
                var dockPosition = AutoHideBar?.Dock;
                return dockPosition == Dock.Right || dockPosition == Dock.Top;
            }
        }

        static AutoHideTab()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(AutoHideTab), new FrameworkPropertyMetadata(typeof(AutoHideTab)));

            // When navigating with the keyboard focus auto-hide tab only once. Don't step inside.
            KeyboardNavigation.DirectionalNavigationProperty.OverrideMetadata(typeof(AutoHideTab), new FrameworkPropertyMetadata(KeyboardNavigationMode.Once));
            KeyboardNavigation.TabNavigationProperty.OverrideMetadata(typeof(AutoHideTab), new FrameworkPropertyMetadata(KeyboardNavigationMode.Local));
        }

        public AutoHideTab()
        {
            _hoverTimer = new DispatcherTimer { Interval = SystemParameters.MouseHoverTime };
            _hoverTimer.Tick += OnMouseHover;
        }

        public override void OnApplyTemplate()
        {
            UpdateVisualStates(false);
        }

        protected override void OnGotFocus(RoutedEventArgs e)
        {
            AutoHideBar?.TabEnter(this);
            base.OnGotFocus(e);
        }

        protected override void OnLostFocus(RoutedEventArgs e)
        {
            AutoHideBar?.TabLeave(this);
            base.OnLostFocus(e);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            base.OnKeyDown(e);

            if (!e.Handled)
            {
                if ((e.Key == Key.Enter || e.Key == Key.Space))
                    AutoHideBar?.TabClicked(this);

                e.Handled = true;
            }
        }

        protected override void OnMouseLeftButtonDown(MouseButtonEventArgs e)
        {
            base.OnMouseLeftButtonDown(e);

            if (!e.Handled)
            {
                AutoHideBar?.TabClicked(this);
                e.Handled = true;
            }
        }

        protected override void OnMouseEnter(MouseEventArgs e)
        {
            // Start timer to measure hover-time.
            _hoverTimer.Start();

            base.OnMouseEnter(e);
            UpdateVisualStates(true);
        }

        protected override void OnMouseLeave(MouseEventArgs e)
        {
            _hoverTimer.Stop();
            AutoHideBar?.TabLeave(this);

            base.OnMouseLeave(e);
            UpdateVisualStates(true);
        }

        private void OnMouseHover(object sender, EventArgs eventArgs)
        {
            _hoverTimer.Stop();
            AutoHideBar?.TabEnter(this);
        }

        private void UpdateVisualStates(bool useTransitions)
        {
            if (IsEnabled)
            {
                if (IsMouseOver)
                    VisualStateManager.GoToState(this, "MouseOver", useTransitions);
                else
                    VisualStateManager.GoToState(this, "Normal", useTransitions);
            }
            else
            {
                VisualStateManager.GoToState(this, "Disabled", useTransitions);
            }

            if (IsUpsideDown)
                VisualStateManager.GoToState(this, "UpsideDown", useTransitions);
            else
                VisualStateManager.GoToState(this, "Upright", useTransitions);
        }
    }
}
