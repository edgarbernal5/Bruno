using System;
using System.Threading;
using System.Threading.Tasks;

namespace EsteroFramework
{
    public interface IActivate
    {
        bool IsActive { get; }

        void Activate();

        event EventHandler<ActivationEventArgs> Activated;
    }

    public class ActivationEventArgs : EventArgs
    {
        public bool WasInitialized { get; set; }
    }
}