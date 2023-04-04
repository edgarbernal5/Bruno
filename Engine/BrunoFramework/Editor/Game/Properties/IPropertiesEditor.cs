
namespace BrunoFramework.Editor.Game.Properties
{
    public interface IPropertiesEditor : IProperties
    {
        BoundPropertyDescriptor BoundPropertyDescriptor { get; set; }
    }
}
