
using System.Threading.Tasks;

namespace BrunoFramework
{
    public interface IClose
    {
        Task TryCloseAsync(bool? dialogResult = null);
    }
}
