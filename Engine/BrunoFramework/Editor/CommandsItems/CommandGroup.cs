using BrunoWindows;
using System;
using System.Windows.Input;

namespace BrunoFramework.Editor
{
    public class CommandGroup : PropertyChangedBase, ICommandItem
    {
        public string Name { get; private set; }
        public ICommand Command => null;

        public object CommandParameter => null;

        public object Icon => null;

        public string Text
        {
            get => _text;
            set
            {
                _text = value;
                NotifyOfPropertyChange();

            }
        }
        private string _text;

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

        public CommandGroup(string name)
          : this(name, name)
        {
        }

        public CommandGroup(string name, string text)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (text == null)
                throw new ArgumentNullException(nameof(text));

            Name = name;
            _text = text;
        }

        public MenuItemViewModel CreateMenuItem()
        {
            return new MenuItemViewModel(this);
        }
    }
}
