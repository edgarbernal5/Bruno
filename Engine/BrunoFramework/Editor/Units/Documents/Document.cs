
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

        public bool IsUntitled
        {
            get => m_isUntitled;
            protected set
            {
                Set(ref m_isUntitled, value);
            }
        }
        private bool m_isUntitled;

        public bool IsModified
        {
            get => m_isModified;
            protected set
            {
                Set(ref m_isModified, value);
            }
        }
        private bool m_isModified;

        public string FilePath
        {
            get => m_filenamePath;
            protected set
            {
                Set(ref m_filenamePath, value);
            }
        }
        private string m_filenamePath;

        public DocumentType DocumentType { get; private set; }

        public DocumentViewModel ViewModel => m_viewModel;
        private DocumentViewModel m_viewModel;

        public IDocumentService DocumentService
        {
            get => m_documentService;
        }

        private IDocumentService m_documentService;

        public string UntitledName
        {
            get
            {
                if (!string.IsNullOrEmpty(m_untitledName)) return m_untitledName;

                if (!IsUntitled)
                    return string.Empty;

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
            m_documentService = editor.Units.OfType<IDocumentService>().FirstOrDefault();
            m_counterByExtension = new Dictionary<string, int>();

            m_documentService.RecordDocument(this);
        }

        public string GetDisplayName()
        {
            string title = string.Empty;
            if (IsUntitled)
            {
                title = UntitledName;
            } 
            else if (!string.IsNullOrEmpty(FilePath))
            {
                title = Path.GetFileName(FilePath);
            }

            if (IsModified)
            {
                title += "*";
            }

            return title;
        }

        public void New()
        {
            IsUntitled = true;
            IsModified = false;

            FilePath = string.Empty;

            OnNew();
        }

        protected abstract void OnNew();

        public void Load(string filename)
        {
            IsUntitled = false;
            IsModified = false;

            FilePath = filename;

            OnLoad(filename);
        }

        protected abstract void OnLoad(string filename);

        public void Save()
        {
            OnSave();

            IsUntitled = false;
            IsModified = false;
        }

        protected abstract void OnSave();

        public abstract DocumentViewModel CreateViewModel();

        internal void RecordViewModel(DocumentViewModel viewModel)
        {
            m_viewModel = viewModel;
        }

        internal void UnrecordViewModel(DocumentViewModel viewModel)
        {
            m_viewModel = null;
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                m_documentService.UnrecordDocument(this);
            }
            base.OnDisposing(disposing);
        }
    }
}
