using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace TrioWpfFramework.Windows.Docking
{
    [TemplatePart(Name = "PART_ContentPresenter", Type = typeof(ContentPresenter))]
    [TemplateVisualState(GroupName = "ContentStates", Name = "Empty")]
    [TemplateVisualState(GroupName = "ContentStates", Name = "Filled")]
    public class DockAnchorPane : ContentControl
    {
        private ContentPresenter _contentPresenter;

        internal FrameworkElement ChildPane
        {
            get { return _contentPresenter.GetContentContainer<FrameworkElement>(); }
        }

        public static readonly DependencyProperty DockWidthProperty = DockControl.DockWidthProperty.AddOwner(typeof(DockAnchorPane));

        [TypeConverter(typeof(GridLengthConverter))]
        public GridLength DockWidth
        {
            get { return (GridLength)GetValue(DockWidthProperty); }
            set { SetValue(DockWidthProperty, value); }
        }

        public static readonly DependencyProperty DockHeightProperty = DockControl.DockHeightProperty.AddOwner(typeof(DockAnchorPane));

        [TypeConverter(typeof(GridLengthConverter))]
        public GridLength DockHeight
        {
            get { return (GridLength)GetValue(DockHeightProperty); }
            set { SetValue(DockHeightProperty, value); }
        }

        static DockAnchorPane()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(DockAnchorPane), new FrameworkPropertyMetadata(typeof(DockAnchorPane)));
            HorizontalContentAlignmentProperty.OverrideMetadata(typeof(DockAnchorPane), new FrameworkPropertyMetadata(HorizontalAlignment.Stretch));
            VerticalContentAlignmentProperty.OverrideMetadata(typeof(DockAnchorPane), new FrameworkPropertyMetadata(VerticalAlignment.Stretch));

            // DockAnchorPane is not a tab stop, only the DockTabItems inside the DockPanes.
            IsTabStopProperty.OverrideMetadata(typeof(DockAnchorPane), new FrameworkPropertyMetadata(Boxed.BooleanFalse));
        }

        public DockAnchorPane()
        {
            Loaded += OnLoaded;
        }

        public override void OnApplyTemplate()
        {
            _contentPresenter = null;

            base.OnApplyTemplate();

            _contentPresenter = GetTemplateChild("PART_ContentPresenter") as ContentPresenter;
            UpdateVisualStates(false);
        }

        protected override void OnContentChanged(object oldContent, object newContent)
        {
            var oldDockPane = oldContent as INotifyPropertyChanged;
            if (oldDockPane != null)
                PropertyChangedEventManager.RemoveHandler(oldDockPane, OnContentVisibilityChanged, nameof(IDockPane.IsVisible));

            base.OnContentChanged(oldContent, newContent);

            var newDockPane = newContent as INotifyPropertyChanged;
            if (newDockPane != null)
                PropertyChangedEventManager.AddHandler(newDockPane, OnContentVisibilityChanged, nameof(IDockPane.IsVisible));

            UpdateVisualStates(true);
        }

        private void OnContentVisibilityChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            UpdateVisualStates(true);
        }

        private void OnLoaded(object sender, RoutedEventArgs eventArgs)
        {
            UpdateVisualStates(true);
        }

        private void UpdateVisualStates(bool useTransitions)
        {
            // Show/hide ChildPane.
            var content = Content;
            var dockPane = content as IDockPane;
            bool isContentVisible = dockPane?.IsVisible ?? (content != null);
            if (_contentPresenter != null)
                _contentPresenter.Visibility = isContentVisible ? Visibility.Visible : Visibility.Collapsed;

            // Update visual states.
            if (isContentVisible)
                VisualStateManager.GoToState(this, "Filled", useTransitions);
            else
                VisualStateManager.GoToState(this, "Empty", useTransitions);
        }
    }
}
