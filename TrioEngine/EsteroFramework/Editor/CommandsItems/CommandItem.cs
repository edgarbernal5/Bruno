using EsteroWindows;
using System.Windows.Input;

namespace EsteroFramework.Editor
{
    public class CommandItem : PropertyChangedBase, ICommandItem
    {
        public string Name { get; }
        public ICommand Command { get; protected set; }

        public object CommandParameter
        {
            get => _commandParameter;
            set {
                _commandParameter = value;
                NotifyOfPropertyChange();
            }
        }
        private object _commandParameter;

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

        public bool IsCheckable
        {
            get => _isCheckable;
            set
            {
                _isCheckable = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isCheckable;

        public bool IsChecked
        {
            get => _isChecked;
            set
            {
                _isChecked = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isChecked;

        public CommandItem(string name, ICommand command) : this(name, name, command)
        {

        }

        public CommandItem(string name, string text, ICommand command)
        {
            Name = name;
            _text = text;
            Command = command;
        }

        public virtual MenuItemViewModel CreateMenuItem()
        {
            return new MenuItemViewModel(this);
        }
    }
}
