using System;
using System.Windows.Input;

namespace TrioWpfFramework.Editor
{
    /// <summary>
    /// Defines an item that invokes a <see cref="RoutedCommand"/>.
    /// </summary>
    /// <remarks>
    /// If the <see cref="RoutedCommand"/> is a <see cref="RoutedUICommand"/>, then the
    /// <see cref="CommandItem.Text"/> property is automatically initialized with the
    /// <see cref="RoutedUICommand.Text"/> of the <see cref="RoutedUICommand"/>.
    /// </remarks>
    public class RoutedCommandItem : CommandItem<RoutedCommand>
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="RoutedCommand"/> class.
        /// </summary>
        /// <param name="command">The command.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="command"/> is <see langword="null"/>.
        /// </exception>
        public RoutedCommandItem(RoutedCommand command)
          : base(command?.Name)
        {
            if (command == null)
                throw new ArgumentNullException(nameof(command));

            Command = command;

            // The name, input gestures and the text are taken from the command per default.
            InputGestures = command.InputGestures;
            var routedUICommand = command as RoutedUICommand;
            if (routedUICommand != null)
                Text = routedUICommand.Text;
        }
    }
}
