
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IAsyncCommand : IDelegateCommand
    {
        Task ExecuteAsync();
        bool CanExecute();
    }
}
