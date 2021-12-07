
using System;
using System.Collections.Generic;

namespace BrunoFramework.Editor.Units
{
    public abstract class DocumentFactory
    {
        public List<DocumentFileType> SupportedFileTypes { get; protected set; }

        protected DocumentFactory()
        {
            SupportedFileTypes = new List<DocumentFileType>();
        }

        public Document Create(DocumentFileType projectFileType)
        {
            if (!SupportedFileTypes.Exists(fileType => fileType.Name == projectFileType.Name))
                throw new InvalidOperationException();

            return OnCreate(projectFileType);
        }

        protected abstract Document OnCreate(DocumentFileType projectFileType);
    }
}
