
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IConductor
    {
        Task ActivateItemAsync(object item, CancellationToken cancellationToken = default);

        Task DeactivateItemAsync(object item, bool close, CancellationToken cancellationToken = default);
    }
}
