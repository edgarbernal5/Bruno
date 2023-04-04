
using System;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IDeactivate
    {
        event EventHandler<DeactivationEventArgs> Deactivating;

        Task DeactivateAsync(bool close, CancellationToken cancellationToken = default);

        event EventHandler<DeactivationEventArgs> Deactivated;
    }

    public class DeactivationEventArgs : EventArgs
    {
        public bool WasClosed { get; set; }
    }
}
