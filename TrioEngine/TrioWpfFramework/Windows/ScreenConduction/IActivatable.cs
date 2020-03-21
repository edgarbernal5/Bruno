using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.ScreenConduction
{
    public interface IActivatable
    {
        bool IsOpen { get; }

        bool IsActive { get; }


        /// <summary>
        /// Occurs when the item is opened or activated.
        /// </summary>
        event EventHandler<ActivationEventArgs> Activated;


        /// <summary>
        /// Occurs when the item is deactivated or closed.
        /// </summary>
        event EventHandler<DeactivationEventArgs> Deactivated;

        void OnActivate();

        void OnDeactivate(bool close);
    }

    /// <summary>
    /// Provides the arguments for the <see cref="IActivatable.Activated"/> event.
    /// </summary>
    public class ActivationEventArgs : EventArgs
    {
        /// <summary>
        /// Gets a value indicating whether the sender was opened in addition to being activated.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the sender was opened; otherwise, <see langword="false"/>.
        /// </value>
        public bool Opened { get; }


        /// <summary>
        /// Initializes a new instance of the <see cref="ActivationEventArgs"/> class.
        /// </summary>
        /// <param name="opened">
        /// <see langword="true"/> if the sender was opened; otherwise, <see langword="false"/>.
        /// </param>
        public ActivationEventArgs(bool opened)
        {
            Opened = opened;
        }
    }


    /// <summary>
    /// Provides the arguments for the <see cref="IActivatable.Deactivated"/> event.
    /// </summary>
    public class DeactivationEventArgs : EventArgs
    {
        /// <summary>
        /// Gets a value indicating whether sender is going to be closed in addition to being 
        /// deactivated.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the sender is closed; otherwise, <see langword="false"/>.
        /// </value>
        public bool Closed { get; set; }


        /// <summary>
        /// Initializes a new instance of the <see cref="DeactivationEventArgs"/> class.
        /// </summary>
        /// <param name="closed">
        /// <see langword="true"/> if the sender is closed; otherwise, <see langword="false"/>.
        /// </param>
        public DeactivationEventArgs(bool closed)
        {
            Closed = closed;
        }
    }
}
