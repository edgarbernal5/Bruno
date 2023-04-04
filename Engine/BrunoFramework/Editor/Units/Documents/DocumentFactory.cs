
using System;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public abstract class DocumentFactory
    {
        public List<DocumentType> SupportedFileTypes { get; protected set; }

        protected DocumentFactory()
        {
            SupportedFileTypes = new List<DocumentType>();
        }

        public Document Create(DocumentType documentFileType, Document existingDocument = null)
        {
            if (!SupportedFileTypes.Exists(fileType => fileType.Name == documentFileType.Name))
                throw new InvalidOperationException();

            return OnCreate(documentFileType, existingDocument);
        }

        protected abstract Document OnCreate(DocumentType documentFileType, Document existingDocument);
    }
}
