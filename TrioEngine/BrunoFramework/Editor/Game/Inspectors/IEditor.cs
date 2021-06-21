
namespace BrunoFramework.Editor.Game.Inspectors
{
    public interface IEditor : IInspector
    {
        BoundPropertyDescriptor BoundPropertyDescriptor { get; set; }
    }
}
