
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public class DefaultCloseStrategy<T> : ICloseStrategy<T>
    {
        public DefaultCloseStrategy()
        {
        }

        public async Task<bool> ExecuteAsync(IEnumerable<T> toClose, CancellationToken cancellationToken = default)
        {
            var closeCanOccur = true;

            foreach (var child in toClose)
            {
                var guardClose = child as IGuardClose;
                if (guardClose != null)
                {
                    var canClose = await guardClose.CanCloseAsync(cancellationToken);

                    closeCanOccur = closeCanOccur && canClose;
                }
            }

            return closeCanOccur;
        }
    }
}
