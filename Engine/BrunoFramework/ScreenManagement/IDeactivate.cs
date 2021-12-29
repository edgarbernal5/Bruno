
using System;

namespace BrunoFramework
{
    public interface IDeactivate
    {
        event EventHandler<DeactivationEventArgs> Deactivating;

        void Deactivate(bool close);

        event EventHandler<DeactivationEventArgs> Deactivated;
    }

    public class DeactivationEventArgs : EventArgs
    {
        /// <summary>
        /// Indicates whether the sender was closed in addition to being deactivated.
        /// </summary>
        public bool WasClosed { get; set; }
    }
}
