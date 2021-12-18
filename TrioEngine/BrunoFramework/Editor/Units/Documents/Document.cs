
using BrunoWindows;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace BrunoFramework.Editor.Units
{
    public abstract class Document : PropertyChangedBase, IDisposable
    {
        public IEditorService Editor { get; }

        public bool IsUntitled { get; private set; }
        public bool IsModified { get; private set; }

        public bool IsDisposed { get; private set; }

        public string FilenamePath { get; protected set; }
        public DocumentType DocumentType { get; private set; }

        public string UntitledName
        {
            get
            {
                if (!string.IsNullOrEmpty(m_untitledName)) return m_untitledName;

                if (!IsUntitled)
                    return string.Empty;

                var documentService = Editor.Services.GetInstance<IDocumentService>();

                string extension = DocumentType.FileExtensions.FirstOrDefault() ?? string.Empty;

                int lastUntitledNumber;
                m_counterByExtension.TryGetValue(extension, out lastUntitledNumber);

                lastUntitledNumber++;
                m_counterByExtension[extension] = lastUntitledNumber;

                m_untitledName = $"Untitled {lastUntitledNumber}";
                return m_untitledName;
            }
        }
        private string m_untitledName;
        private Dictionary<string, int> m_counterByExtension;

        protected Document(IEditorService editor, DocumentType documentType)
        {
            Editor = editor;
            DocumentType = documentType;

            m_counterByExtension = new Dictionary<string, int>();
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!IsDisposed)
            {
                if (disposing)
                {
                    
                }

                IsDisposed = true;
            }
        }

        public string GetDisplayName()
        {
            string title = string.Empty;
            if (IsUntitled)
            {
                title = UntitledName;
            } 
            else if (!string.IsNullOrEmpty(FilenamePath))
            {
                title = Path.GetFileName(FilenamePath);
            }

            return title;
        }

        public void New()
        {
            IsModified = false;
            IsUntitled = true;

            FilenamePath = string.Empty;

            OnNew();
        }

        protected abstract void OnNew();

        public void Load(string filename)
        {
            IsModified = false;
            IsUntitled = false;

            FilenamePath = filename;

            OnLoad(filename);
        }

        protected abstract void OnLoad(string filename);

        public void Save()
        {
            OnSave();

            IsModified = false;
            IsUntitled = false;
        }

        protected abstract void OnSave();

        public abstract DocumentViewModel CreateViewModel();
    }
}
