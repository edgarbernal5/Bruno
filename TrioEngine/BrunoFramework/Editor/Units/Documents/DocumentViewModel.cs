
namespace BrunoFramework.Editor.Units
{
    public abstract class DocumentViewModel : EditorDockableTabViewModel
    {
        public Document Document { get; }

        protected DocumentViewModel(Document document)
        {
            Document = document;

            DisplayName = document.GetDisplayName();
        }

    }
}
