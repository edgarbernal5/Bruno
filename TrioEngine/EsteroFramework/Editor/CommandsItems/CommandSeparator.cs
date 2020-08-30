using EsteroWindows;
using System;
using System.Windows.Input;

namespace EsteroFramework.Editor
{
    public class CommandSeparator : PropertyChangedBase, ICommandItem
    {
        public string Name { get; }
        public ICommand Command => null;

        public object CommandParameter => null;

        public object Icon => null;

        public string Text => null;

        public bool IsVisible
        {
            get => _isVisible;
            set
            {
                _isVisible = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isVisible = true;

        public bool IsCheckable => false;

        public bool IsChecked => false;

        public MenuItemViewModel CreateMenuItem()
        {
            return new MenuSeparatorViewModel(this);
        }
    }
}
