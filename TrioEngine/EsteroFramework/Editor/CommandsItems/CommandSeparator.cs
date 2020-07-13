using EsteroWindows;
using System;
using System.Windows.Input;

namespace EsteroFramework.Editor
{
    public class CommandSeparator : PropertyChangedBase, ICommandItem
    {
        public string Name { get; }
        public ICommand Command => null;

        public string Text => null;

        public bool IsVisible { get => true; set => throw new NotImplementedException(); }

        public MenuItemViewModel CreateMenuItem()
        {
            return new MenuSeparatorViewModel(this);
        }
    }
}
