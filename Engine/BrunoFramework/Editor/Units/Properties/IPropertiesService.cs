using BrunoFramework.Editor.Game.Properties;

namespace BrunoFramework.Editor.Units
{
    public interface IPropertiesService
    {
        PropertiesViewModel ViewModel { get; }
        IPropertiesObject SelectedObject { get; set; }
    }
}
