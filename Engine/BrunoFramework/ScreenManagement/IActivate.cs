
using System;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IActivate
    {
        bool IsActive { get; }

        Task ActivateAsync(CancellationToken cancellationToken = default);

        event EventHandler<ActivationEventArgs> Activated;
    }

    public class ActivationEventArgs : EventArgs
    {
        public bool WasInitialized { get; set; }
    }
}