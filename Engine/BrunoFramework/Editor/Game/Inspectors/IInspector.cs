
namespace BrunoFramework.Editor.Game.Inspectors
{
    public interface IInspector
    {
        string Name { get; }
        bool IsReadOnly { get; }
    }
}
