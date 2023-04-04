
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface ICloseStrategy<T>
    {
        Task<bool> ExecuteAsync(IEnumerable<T> toClose, CancellationToken cancellationToken = default);
    }
}