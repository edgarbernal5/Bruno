using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace TrioWpfFramework.Editor.Commands
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
