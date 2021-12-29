
namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserViewModel : EditorDockableTabViewModel
    {
        internal const string DockIdString = "ContentBrowser";

        public ContentBrowser Content
        {
            get => m_content;
            set
            {
                m_content = value;
                NotifyOfPropertyChange();
            }
        }
        private ContentBrowser m_content;

        private IEditorService m_editor;

        public ContentBrowserViewModel(IEditorService editor) : base()
        {
            DisplayName = "Content Browser";
            DockId = DockIdString;
            m_editor = editor;
        }
    }
}
