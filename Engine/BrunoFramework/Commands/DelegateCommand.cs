using System;
using System.Windows.Input;

namespace BrunoFramework
{
    public class DelegateCommand : IDelegateCommand
    {
        private readonly Action _execute;
        private readonly Func<bool> _canExecute;

        public event EventHandler CanExecuteChanged;

        public DelegateCommand(Action execute, Func<bool> canExecute = null)
        {
            if (execute == null)
                throw new ArgumentNullException(nameof(execute));

            _execute = execute;

            if (canExecute != null)
            {
                _canExecute = canExecute;
            }
        }

        bool ICommand.CanExecute(object parameter)
        {
            return CanExecute();
        }

        void ICommand.Execute(object parameter)
        {
            Execute();
        }

        public bool CanExecute()
        {
            return _canExecute == null || _canExecute();
        }

        public void Execute()
        {
            _execute();
        }

        public void RaiseCanExecuteChanged()
        {
            OnCanExecuteChanged();
        }

        protected virtual void OnCanExecuteChanged()
        {
            CommandManager.InvalidateRequerySuggested();
            CanExecuteChanged?.Invoke(this, EventArgs.Empty);
        }
    }

    public class DelegateCommand<T> : IDelegateCommand
    {
        // https://msdn.microsoft.com/en-us/library/ms366789.aspx
        private static readonly bool IsNotNullableValueType =
            typeof(T).IsValueType && !(typeof(T).IsGenericType && typeof(T).GetGenericTypeDefinition() == typeof(Nullable<>));

        private readonly Action<T> _execute;
        private readonly Func<T, bool> _canExecute;

        public event EventHandler CanExecuteChanged;

        public DelegateCommand(Action<T> execute, Func<T, bool> canExecute = null)
        {
            if (execute == null)
                throw new ArgumentNullException(nameof(execute));

            _execute = execute;
            if (canExecute != null)
            {
                _canExecute = canExecute;
            }
        }

        bool ICommand.CanExecute(object parameter)
        {
            if (parameter == null && IsNotNullableValueType)
                return false;

            return _canExecute == null || _canExecute((T)parameter);
        }

        void ICommand.Execute(object parameter)
        {
            Execute((T)parameter);
        }

        public bool CanExecute(T parameter)
        {
            return _canExecute == null || _canExecute(parameter);
        }

        public void Execute(T parameter)
        {
            _execute(parameter);
        }

        public void RaiseCanExecuteChanged()
        {
            OnCanExecuteChanged();
        }

        protected virtual void OnCanExecuteChanged()
        {
            //CommandManager.InvalidateRequerySuggested();
            CanExecuteChanged?.Invoke(this, EventArgs.Empty);
        }
    }
}

