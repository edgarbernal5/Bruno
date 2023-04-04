using BrunoFramework.Editor.Game.Properties.Converters;

namespace BrunoFramework.Editor.Game.Properties
{
    public class QuaternionEditorViewModel : Vector3EditorViewModel
    {
        public QuaternionEditorViewModel()
        {
            Converter = new QuaternionToEulerAngles();
        }
    }
}
