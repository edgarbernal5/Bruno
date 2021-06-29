
namespace BrunoFramework.Editor.Game.Inspectors
{
    public interface IInspectorEditor : IInspector
    {
        BoundPropertyDescriptor BoundPropertyDescriptor { get; set; }
    }
}
