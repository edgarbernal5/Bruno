
namespace EsteroFramework.Editor.Units
{
    public class ProjectFileViewModel : Screen
    {
        public ProjectFile ProjectFile { get; }

        protected ProjectFileViewModel(ProjectFile projectFile)
        {
            ProjectFile = projectFile;
        }

    }
}
