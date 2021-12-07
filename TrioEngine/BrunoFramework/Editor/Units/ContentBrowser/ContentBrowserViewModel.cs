
namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserViewModel : Screen
    {
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

        public ContentBrowserViewModel(IEditorService editor)
        {
            m_editor = editor;
        }
    }
}
