using System.Windows.Input;

namespace EsteroFramework
{
    public interface IDelegateCommand : ICommand
    {
        /// <summary>
        /// Raises <see cref="DelegateCommand.CanExecuteChanged"/> so every command invoker can
        /// requery to check if the <see cref="DelegateCommand"/> can execute.
        /// </summary>
        /// <remarks>
        /// Note that this will trigger the execution of <see cref="DelegateCommand.CanExecute"/>
        /// once for each invoker.
        /// </remarks>
        void RaiseCanExecuteChanged();
    }

}
