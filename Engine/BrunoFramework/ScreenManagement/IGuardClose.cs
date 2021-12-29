
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IGuardClose
    {
        Task<bool> CanCloseAsync(CancellationToken cancellationToken = default);
    }
}
