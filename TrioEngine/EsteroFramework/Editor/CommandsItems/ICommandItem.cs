using Estero.Collections;
using System.Windows.Input;

namespace EsteroFramework.Editor
{
    public interface ICommandItem : INamedItem
    {
        ICommand Command { get; }

        string Text { get; }

        bool IsVisible { get; set; }

        MenuItemViewModel CreateMenuItem();
    }
}
