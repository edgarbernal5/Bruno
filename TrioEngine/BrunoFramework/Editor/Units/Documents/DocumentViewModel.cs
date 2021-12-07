
namespace BrunoFramework.Editor.Units
{
    public class DocumentViewModel : Screen
    {
        public Document ProjectFile { get; }

        protected DocumentViewModel(Document projectFile)
        {
            ProjectFile = projectFile;
        }

    }
}
