
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public interface IDocumentService
    {
        List<DocumentFactory> Factories { get; }

        Document New(DocumentType projectFileType);

        bool CanOpen(string filename);

        Document Open(string filename);

        bool ShowSaveChangesDialog(Document document);

        bool Close(Document document, bool force);

        Document ActiveDocument { get; }

        void RecordDocument(Document document);

        void UnrecordDocument(Document document);
    }
}
