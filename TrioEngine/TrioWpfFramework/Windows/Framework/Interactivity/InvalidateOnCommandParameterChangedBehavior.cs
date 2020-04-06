using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;

namespace TrioWpfFramework.Windows.Framework
{
    /// <summary>
    /// Requeries the "can execute" state of the command when the command parameter is changed.
    /// </summary>
    /// <remarks>
    /// <para>
    /// WPF does not monitor command parameter changes and does not requery the CanExecute state
    /// of commands when the command parameter changes. This is a problem when a command parameter
    /// is determined using data binding. For example: The user opens a context menu. The 
    /// CanExecute state of commands is automatically checked. After that the bindings of menu items 
    /// are evaluated. If the command parameter of a menu item is determined by a binding, then
    /// the CanExecute state of the command has to be checked again. This does not happen 
    /// automatically, and therefore you have to add this behavior to the menu item. 
    /// </para>
    /// <para>
    /// This behavior can be used on <see cref="FrameworkElement"/>s which implement
    /// <see cref="ICommandSource"/>. The command can be a <see cref="IDelegateCommand"/> or
    /// a <see cref="RoutedCommand"/>.
    /// </para>
    /// </remarks>
    public class InvalidateOnCommandParameterChangedBehavior : Behavior<FrameworkElement>
    {
        private BindablePropertyObserver _propertyObserver;
        
        /// <summary>
        /// Called after the behavior is attached to an <see cref="Behavior{T}.AssociatedObject"/>.
        /// </summary>
        /// <remarks>
        /// Override this to hook up functionality to the
        /// <see cref="Behavior{T}.AssociatedObject"/>.
        /// </remarks>
        protected override void OnAttached()
        {
            base.OnAttached();

            var commandSource = AssociatedObject as ICommandSource;
            if (commandSource == null)
                return;

            _propertyObserver = new BindablePropertyObserver(AssociatedObject, "CommandParameter");
            _propertyObserver.ValueChanged += OnCommandParameterChanged;
        }


        /// <summary>
        /// Called when the <see cref="Behavior{T}"/> is about to detach from the
        /// <see cref="Behavior{T}.AssociatedObject"/>.
        /// </summary>
        /// <remarks>
        /// When this method is called, detaching can not be canceled. The
        /// <see cref="Behavior{T}.AssociatedObject"/> is still set.
        /// </remarks>
        protected override void OnDetaching()
        {
            if (_propertyObserver != null)
            {
                _propertyObserver.Dispose();
                _propertyObserver = null;
            }

            base.OnDetaching();
        }


        private void OnCommandParameterChanged(object sender, EventArgs eventArgs)
        {
            var commandSource = AssociatedObject as ICommandSource;
            if (commandSource == null)
                return;

            var delegateCommand = commandSource.Command as IDelegateCommand;
            if (delegateCommand != null)
            {
                delegateCommand.RaiseCanExecuteChanged();
                return;
            }

            var routedCommand = commandSource.Command as RoutedCommand;
            if (routedCommand != null)
            {
                CommandManager.InvalidateRequerySuggested();
                return;
            }
        }
    }
}