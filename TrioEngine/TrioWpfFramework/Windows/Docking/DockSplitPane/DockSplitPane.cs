using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using TrioWpfFramework.Windows.Docking.Controls;
using TrioWpfFramework.Windows.Docking.ViewModels;

namespace TrioWpfFramework.Windows.Docking
{
    /// <summary>
    /// Represents a pane in the docking layout that is split horizontally or vertically into two
    /// or more panes.
    /// </summary>
    public class DockSplitPane : ItemsControl
    {
        // Workaround for creating item containers.
        private object _currentItem;

        public static readonly DependencyProperty DockWidthProperty = DockControl.DockWidthProperty.AddOwner(typeof(DockSplitPane));

        [TypeConverter(typeof(GridLengthConverter))]
        public GridLength DockWidth
        {
            get { return (GridLength)GetValue(DockWidthProperty); }
            set { SetValue(DockWidthProperty, value); }
        }

        public static readonly DependencyProperty DockHeightProperty = DockControl.DockHeightProperty.AddOwner(typeof(DockSplitPane));

        [TypeConverter(typeof(GridLengthConverter))]
        public GridLength DockHeight
        {
            get { return (GridLength)GetValue(DockHeightProperty); }
            set { SetValue(DockHeightProperty, value); }
        }

        public static readonly DependencyProperty OrientationProperty = DependencyProperty.Register(
            "Orientation",
            typeof(Orientation),
            typeof(DockSplitPane),
            new FrameworkPropertyMetadata(Orientation.Horizontal, FrameworkPropertyMetadataOptions.AffectsMeasure));

        public Orientation Orientation
        {
            get { return (Orientation)GetValue(OrientationProperty); }
            set { SetValue(OrientationProperty, value); }
        }

        static DockSplitPane()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(DockSplitPane), new FrameworkPropertyMetadata(typeof(DockSplitPane)));
        }

        protected override void OnItemsSourceChanged(IEnumerable oldValue, IEnumerable newValue)
        {
            base.OnItemsSourceChanged(oldValue, newValue);

            if (newValue != null)
            {
                // Filter IDockPanes that are invisible.
                var collectionView = CollectionViewSource.GetDefaultView(newValue);
                var collectionViewLiveShaping = collectionView as ICollectionViewLiveShaping;
                if (collectionViewLiveShaping != null && collectionViewLiveShaping.CanChangeLiveFiltering)
                {
                    collectionViewLiveShaping.LiveFilteringProperties.Clear();
                    collectionViewLiveShaping.LiveFilteringProperties.Add(nameof(IDockPane.IsVisible));
                    collectionViewLiveShaping.IsLiveFiltering = true;
                    collectionView.Filter = Filter;
                }
            }
        }


        private static bool Filter(object item)
        {
            var dockPane = item as IDockPane;
            return dockPane == null || dockPane.IsVisible;
        }


        /// <summary>
        /// Determines if the specified item is (or is eligible to be) its own container.
        /// </summary>
        /// <param name="item">The item to check.</param>
        /// <returns>
        /// <see langword="true"/> if the item is (or is eligible to be) its own container;
        /// otherwise, <see langword="false"/>.
        /// </returns>
        protected override bool IsItemItsOwnContainerOverride(object item)
        {
            if (item is DockTabPane || item is DockSplitPane || item is DockAnchorPane)
            {
                _currentItem = null;
                return true;
            }

            // Store the current item for use in GetContainerForItemOverride.
            // (The base ItemsControl will call IsItemItsOwnContainerOverride followed by
            // GetContainerForItemOverride if the result was false.)
            _currentItem = item;
            return false;
        }


        protected override DependencyObject GetContainerForItemOverride()
        {
            if (_currentItem != null)
            {
                var type = _currentItem.GetType();
                _currentItem = null;

                // Manually load implicit data template. (Otherwise the ItemsControl will create a
                // ContentPresenter.)
                var dataTemplateKey = new DataTemplateKey(type);
                var dataTemplate = TryFindResource(dataTemplateKey) as DataTemplate;
                var container = dataTemplate?.LoadContent();
                if (container is DockAnchorPane || container is DockSplitPane || container is DockTabPane)
                    return container;
            }

            // Fix for Visual Studio Designer.
            if (WpfWindowsExtensions.IsInDesignMode)
                return base.GetContainerForItemOverride();

            throw new DockException("Items in DockSplitPane need to be of type DockAnchorPane/DockSplitPane/DockTabPane "
                                    + "or need to have an implicit data template of the given type.");
        }

    }
}
