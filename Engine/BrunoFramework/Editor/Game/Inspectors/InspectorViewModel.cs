
namespace BrunoFramework.Editor.Game.Inspectors
{
    public class InspectorViewModel : EditorDockableTabViewModel
    {
        internal const string DockIdString = "Inspector";

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
            : base()
        {
            DisplayName = "Inspector";
            DockId = DockIdString;
        }
    }
}
