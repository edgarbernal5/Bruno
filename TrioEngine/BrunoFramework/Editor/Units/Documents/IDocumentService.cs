
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public interface IDocumentService
    {
        List<DocumentFactory> Factories { get; }

        Document New(DocumentType projectFileType);
        Document Open(string filename);
    }
}
