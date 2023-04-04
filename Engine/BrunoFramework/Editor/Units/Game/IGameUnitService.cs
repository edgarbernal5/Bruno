
namespace BrunoFramework.Editor.Units
{
    public interface IGameUnitService
    {
        event GameUnitEvent OnNewFrame;
    }

    public delegate void GameUnitEvent();
}
