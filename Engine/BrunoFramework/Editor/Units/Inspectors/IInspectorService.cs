using BrunoFramework.Editor.Game.Inspectors;

namespace BrunoFramework.Editor.Units
{
    public interface IInspectorService
    {
        InspectorViewModel ViewModel { get; }
        IInspectableObject SelectedObject { get; set; }
    }
}
