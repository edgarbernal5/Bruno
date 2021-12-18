
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

        public Document Create(DocumentType projectFileType)
        {
            if (!SupportedFileTypes.Exists(fileType => fileType.Name == projectFileType.Name))
                throw new InvalidOperationException();

            return OnCreate(projectFileType);
        }

        protected abstract Document OnCreate(DocumentType projectFileType);
    }
}
