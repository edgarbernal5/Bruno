using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.ScreenConduction
{
    public interface IGuardClose
    {
        Task<bool> CanCloseAsync();
    }
}
