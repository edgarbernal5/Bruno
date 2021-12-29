using BrunoFramework.Editor.Game.Inspectors.Converters;

namespace BrunoFramework.Editor.Game.Inspectors
{
    public class QuaternionEditorViewModel : Vector3EditorViewModel
    {
        public QuaternionEditorViewModel()
        {
            Converter = new QuaternionToEulerAngles();
        }
    }
}
