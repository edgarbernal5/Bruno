
namespace BrunoFramework.Editor.Units
{
    public class DocumentType
    {
        public string Name { get; }
        public string[] FileExtensions { get; }
        public DocumentFactory Factory { get; }

        public DocumentType(string name, string[] fileExtensions, DocumentFactory factory)
        {
            Name = name;
            FileExtensions = fileExtensions;
            Factory = factory;
        }
    }
}
