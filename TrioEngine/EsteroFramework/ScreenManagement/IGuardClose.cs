
using System.Threading;
using System.Threading.Tasks;

namespace EsteroFramework
{
    public interface IGuardClose
    {
        Task<bool> CanCloseAsync(CancellationToken cancellationToken = default);
    }
}
