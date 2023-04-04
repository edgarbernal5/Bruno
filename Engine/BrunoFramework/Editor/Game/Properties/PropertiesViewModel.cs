
namespace BrunoFramework.Editor.Game.Properties
{
    public class PropertiesViewModel : EditorDockTabViewModel
    {
        internal const string DockIdString = "Properties";

        public IPropertiesObject SelectedObject
        {
            get => _selectedObject;
            set
            {
                _selectedObject = value;
                NotifyOfPropertyChange(() => SelectedObject);
            }
        }
        private IPropertiesObject _selectedObject;

        public PropertiesViewModel(IEditorService editor)
            : base()
        {
            DisplayName = "Properties";
            DockId = DockIdString;
        }
    }
}
