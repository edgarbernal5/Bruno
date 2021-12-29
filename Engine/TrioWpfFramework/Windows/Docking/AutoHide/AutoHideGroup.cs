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

namespace TrioWpfFramework.Windows.Docking
{
    public class AutoHideGroup : ItemsControl
    {
        static AutoHideGroup()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(AutoHideGroup), new FrameworkPropertyMetadata(typeof(AutoHideGroup)));
        }

        protected override void OnItemsSourceChanged(IEnumerable oldValue, IEnumerable newValue)
        {
            base.OnItemsSourceChanged(oldValue, newValue);

            if (newValue != null)
            {
                // Filter IDockTabItems that are invisible.
                var collectionView = CollectionViewSource.GetDefaultView(newValue);
                var collectionViewLiveShaping = collectionView as ICollectionViewLiveShaping;
                if (collectionViewLiveShaping != null && collectionViewLiveShaping.CanChangeLiveFiltering)
                {
                    collectionViewLiveShaping.LiveFilteringProperties.Clear();
                    collectionViewLiveShaping.LiveFilteringProperties.Add(nameof(IDockTabItem.DockState));
                    collectionViewLiveShaping.IsLiveFiltering = true;
                    collectionView.Filter = Filter;
                }
            }
        }


        private static bool Filter(object item)
        {
            var dockTabItem = item as IDockTabItem;
            return dockTabItem == null || dockTabItem.DockState == DockState.AutoHide;
        }

        protected override bool IsItemItsOwnContainerOverride(object item)
        {
            return item is AutoHideTab;
        }

        protected override DependencyObject GetContainerForItemOverride()
        {
            return new AutoHideTab();
        }
    }
}
