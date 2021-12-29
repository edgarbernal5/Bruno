using BrunoWindows;

namespace BrunoFramework.Editor.Game.Inspectors
{
    public abstract class InspectorBase : PropertyChangedBase, IInspector
    {
        public abstract string Name { get; }
        public abstract bool IsReadOnly { get; }
    }
}
