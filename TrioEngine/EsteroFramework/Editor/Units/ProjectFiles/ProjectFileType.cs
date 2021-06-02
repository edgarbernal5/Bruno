
namespace EsteroFramework.Editor.Units
{
    public class ProjectFileType
    {
        public string Name { get; }
        public string[] FileExtensions { get; }
        public ProjectFileFactory Factory { get; }

        public ProjectFileType(string name, string[] fileExtensions, ProjectFileFactory factory)
        {
            Name = name;
            FileExtensions = fileExtensions;
            Factory = factory;
        }
    }
}
