
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public interface IDocumentService
    {
        List<DocumentFactory> Factories { get; }

        Document New(DocumentType projectFileType);
        Document Open(string filename);

        bool CanClose(Document document);
        bool Close(Document document, bool force);

        Document ActiveDocument { get; }
    }
}
