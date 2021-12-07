
namespace BrunoFramework.Editor.Units
{
    public class DocumentFileType
    {
        public string Name { get; }
        public string[] FileExtensions { get; }
        public DocumentFactory Factory { get; }

        public DocumentFileType(string name, string[] fileExtensions, DocumentFactory factory)
        {
            Name = name;
            FileExtensions = fileExtensions;
            Factory = factory;
        }
    }
}
