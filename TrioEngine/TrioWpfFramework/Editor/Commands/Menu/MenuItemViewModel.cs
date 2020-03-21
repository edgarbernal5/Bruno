﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Delivery.ViewModels.Core;
using TrioWpfFramework.Editor.Commands.Menu;

namespace TrioWpfFramework.Editor.Commands
{
    /// <summary>
    /// Represents a command item in a menu.
    /// </summary>
    public class MenuItemViewModel : ObservableObject
    {
        public ICommandItem CommandItem { get; }

        public bool IsVisible
        {
            get { return _isVisible; }

            // The MenuManager hides empty sub-menus and unnecessary separators.
            internal set { SetProperty(ref _isVisible, value); }
        }
        private bool _isVisible = true;

        public MenuItemViewModelCollection Submenu
        {
            get { return _submenu; }
            set { SetProperty(ref _submenu, value); }
        }
        private MenuItemViewModelCollection _submenu;

        public MenuItemViewModel(ICommandItem commandItem)
        {
            if (commandItem == null)
                throw new ArgumentNullException(nameof(commandItem));

            CommandItem = commandItem;
        }
    }
}
