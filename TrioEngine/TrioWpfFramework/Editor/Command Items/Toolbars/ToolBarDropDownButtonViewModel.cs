﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Represents drop-down button in a toolbar.
    /// </summary>
    public class ToolBarDropDownButtonViewModel : ToolBarItemViewModel
    {
        /// <summary>
        /// Gets or sets the width in device-independent pixels.
        /// </summary>
        /// <value>The width in device-independent pixels. The default value is NaN.</value>
        public double Width
        {
            get { return _width; }
            set { SetProperty(ref _width, value); }
        }
        private double _width = double.NaN;


        /// <summary>
        /// Gets or sets the selected item.
        /// </summary>
        /// <value>The selected item.</value>
        public ICommandItem SelectedItem
        {
            get { return _selectedItem; }
            set { SetProperty(ref _selectedItem, value); }
        }
        private ICommandItem _selectedItem;


        /// <summary>
        /// Gets or sets the items shown in the drop-down.
        /// </summary>
        /// <value>The items shown in the drop-down.</value>
        public MenuItemViewModelCollection Items
        {
            get { return _items; }
            set { SetProperty(ref _items, value); }
        }
        private MenuItemViewModelCollection _items;


        /// <summary>
        /// Initializes a new instance of the <see cref="ToolBarSplitButtonViewModel"/> class.
        /// </summary>
        /// <param name="commandItem">The command item. Must not be <see langword="null"/>.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="commandItem"/> is <see langword="null"/>.
        /// </exception>
        public ToolBarDropDownButtonViewModel(ICommandItem commandItem)
            : base(commandItem)
        {
        }
    }
}
