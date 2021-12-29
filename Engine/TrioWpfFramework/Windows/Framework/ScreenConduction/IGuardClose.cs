using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Framework
{
    public interface IGuardClose
    {
        Task<bool> CanCloseAsync();
    }
}
