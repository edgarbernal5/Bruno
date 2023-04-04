
namespace BrunoFramework
{
    public interface IScreen : IHaveDisplayName, IActivate, IDeactivate, IGuardClose, IClose
    {
        IConductor Conductor { get; set; }
    }
}
