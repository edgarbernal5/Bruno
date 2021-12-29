
using System.Windows.Input;

namespace BrunoFramework
{
    public interface IDelegateCommand : ICommand
    {
        void RaiseCanExecuteChanged();
    }
}
