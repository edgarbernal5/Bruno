
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public static class ScreenHelper
    {
        public static Task TryDeactivateAsync(object potentialDeactivatable, bool close, CancellationToken cancellationToken)
        {
            var deactivator = potentialDeactivatable as IDeactivate;
            if (deactivator != null)
            {
                return deactivator.DeactivateAsync(close, cancellationToken);
            }
            return Task.FromResult(true);
        }

        public static Task TryActivateAsync(object potentialActivatable, CancellationToken cancellationToken)
        {
            var activator = potentialActivatable as IActivate;
            if (activator != null)
            {
                return activator.ActivateAsync(cancellationToken);
            }
            return Task.FromResult(true);
        }
    }
}
