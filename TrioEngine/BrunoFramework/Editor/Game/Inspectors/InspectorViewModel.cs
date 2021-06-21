
namespace BrunoFramework.Editor.Game.Inspectors
{
    public class InspectorViewModel : Screen
    {
        public IInspectableObject SelectedObject
        {
            get => _selectedObject;
            set
            {
                _selectedObject = value;
                NotifyOfPropertyChange(() => SelectedObject);
            }
        }
        private IInspectableObject _selectedObject;

        public InspectorViewModel(IEditorService editor)
        {
        }
    }
}
