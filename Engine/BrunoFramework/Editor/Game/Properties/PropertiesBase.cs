using BrunoWindows;

namespace BrunoFramework.Editor.Game.Properties
{
    public abstract class PropertiesBase : PropertyChangedBase, IProperties
    {
        public abstract string Name { get; }
        public abstract bool IsReadOnly { get; }
    }
}
