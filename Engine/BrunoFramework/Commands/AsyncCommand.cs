
using System;
using System.Threading.Tasks;
using System.Windows.Input;

namespace BrunoFramework
{
    public class AsyncCommand : IAsyncCommand
    {
        public event EventHandler CanExecuteChanged;

        private bool m_isExecuting;
        private readonly Func<Task> m_execute;
        private readonly Func<bool> m_canExecute;

        public AsyncCommand(Func<Task> execute, Func<bool> canExecute = null)
        {
            m_execute = execute;
            m_canExecute = canExecute;
        }

        public bool CanExecute() => !m_isExecuting && (m_canExecute?.Invoke() ?? true);

        public async Task ExecuteAsync()
        {
            if (CanExecute())
            {
                try
                {
                    m_isExecuting = true;
                    await m_execute();
                }
                finally
                {
                    m_isExecuting = false;
                }
            }

            RaiseCanExecuteChanged();
        }

        public void RaiseCanExecuteChanged() => CanExecuteChanged?.Invoke(this, EventArgs.Empty);

        bool ICommand.CanExecute(object parameter) => CanExecute();

        void ICommand.Execute(object parameter)
        {
            TryExecuteTask(ExecuteAsync());
        }

        private static async void TryExecuteTask(Task task)
        {
            try
            {
                await task;
            }
            catch
            {
                throw;
            }
        }
    }
}
