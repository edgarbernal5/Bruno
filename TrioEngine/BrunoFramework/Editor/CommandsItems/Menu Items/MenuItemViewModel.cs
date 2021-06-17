using BrunoWindows;
using System;

namespace BrunoFramework.Editor
{
    public class MenuItemViewModel : PropertyChangedBase
    {
        public ICommandItem CommandItem { get; }

        public MenuItemViewModelCollection Submenu
        {
            get => _submenu;
            set 
            {
                _submenu = value;
                NotifyOfPropertyChange();
            }
        }
        private MenuItemViewModelCollection _submenu;

        public bool IsVisible
        {
            get => _isVisible;
            internal set
            {
                _isVisible = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isVisible = true;

        public MenuItemViewModel(ICommandItem commandItem)
        {
            if (commandItem == null)
                throw new ArgumentNullException(nameof(commandItem));

            CommandItem = commandItem;
        }
    }
}
