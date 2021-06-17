using Bruno.Collections;
using System.Windows.Input;

namespace BrunoFramework.Editor
{
    public interface ICommandItem : INamedItem
    {
        ICommand Command { get; }

        object CommandParameter { get; }

        object Icon { get; }

        string Text { get; }

        bool IsVisible { get; set; }

        bool IsCheckable { get; }

        bool IsChecked { get; }

        MenuItemViewModel CreateMenuItem();
    }
}
