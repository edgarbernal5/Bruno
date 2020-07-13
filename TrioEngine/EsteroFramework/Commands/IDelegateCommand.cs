using System.Windows.Input;

namespace EsteroFramework
{
    public interface IDelegateCommand : ICommand
    {
        void RaiseCanExecuteChanged();
    }

}
