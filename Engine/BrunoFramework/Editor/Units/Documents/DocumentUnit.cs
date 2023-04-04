
using Bruno.Collections;
using Bruno.Linq;
using Bruno.Logging;
using Microsoft.Win32;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Threading;

namespace BrunoFramework.Editor.Units
{
    public class DocumentUnit : EditorUnit, IDocumentService, IGuardClose
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        private TreeNodeCollection<ICommandItem> m_menuNodes;
        private IContentBrowserService m_contentBrowserService;

        public List<DocumentFactory> Factories { get => m_factories; }

        public Document ActiveDocument
        {
            get => m_activeDocument;
            internal set
            {
                m_activeDocument = value;
            }
        }
        private Document m_activeDocument;

        private List<DocumentFactory> m_factories;
        private List<Document> m_documents;


        private IWindowManager _windowManager;


        public DocumentUnit()
        {
            m_factories = new List<DocumentFactory>();
            m_documents = new List<Document>();
        }

        private void OnEditorActiveItemChanged(object sender, System.EventArgs eventArgs)
        {
            var documentViewModel = Editor.ActiveItem as DocumentViewModel;
            if (documentViewModel != null)
            {
                ActiveDocument = documentViewModel.Document;
            }

            UpdateCommands();
        }

        private void UpdateCommands()
        {
            CommandItems.Select(command => command.Command)
                        .OfType<IDelegateCommand>()
                        .ForEach(command => command.RaiseCanExecuteChanged());
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IDocumentService), null, this);
            _windowManager = Editor.Services.GetInstance<IWindowManager>();

            Editor.Services.RegisterView(typeof(SaveChangesViewModel), typeof(SaveChangesView));

            Editor.ActiveItemChanged += OnEditorActiveItemChanged;
        }

        protected override void OnStartup()
        {
            m_contentBrowserService = Editor.Services.GetInstance<IContentBrowserService>();
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("Scene", new DelegateCommand(() => NewSceneDocument()))
                {
                    Text = "Scene",
                }
            );
            CommandItems.Add(
                new CommandItem("Open", new DelegateCommand(() => OpenSceneDocument()))
                {
                    Text = "Open",
                }
            );
            CommandItems.Add(
                new CommandItem("Save", new DelegateCommand(() => SaveDocument(), CanSaveDocument))
                {
                    Text = "Save",
                }
            );
            CommandItems.Add(
                new CommandItem("Close", new AsyncCommand(() => Close(), HasActiveDocument))
                {
                    Text = "Close",
                }
            );
            CommandItems.Add(
                new CommandItem("CloseAll", new AsyncCommand(() => CloseAllDocumentsAsync(), HasActiveDocument))
                {
                    Text = "Close all",
                }
            );
            CommandItems.Add(
                new CommandItem("SaveAll", new DelegateCommand(() => SaveAllDocuments(), CanSaveAllDocuments))
                {
                    Text = "Save all",
                }
            );
        }

        private Task Close()
        {
            if (m_activeDocument == null)
                return Task.FromResult(true);

            //Close(m_activeDocument, false);
            return m_activeDocument.ViewModel.TryCloseAsync();
        }

        private bool HasActiveDocument()
        {
            return m_activeDocument != null;
        }

        private void SaveDocument()
        {
            if (m_activeDocument == null)
                return;

            Save(m_activeDocument);
        }

        private void SaveAllDocuments()
        {
            if (m_documents.Count == 0)
                return;

            var modifiedDocuments = m_documents.Where(document => document.IsModified);
            SaveAll(modifiedDocuments);
        }

        private bool CanSaveDocument()
        {
            return m_activeDocument != null && m_activeDocument.IsModified;
        }

        private bool CanSaveAllDocuments()
        {
            return m_documents.Count > 0 && m_documents.Any(document => document.IsModified);
        }

        private void NewSceneDocument()
        {
            var sceneType = m_factories.SelectMany(factory => factory.SupportedFileTypes)
                .FirstOrDefault(documentType => documentType.Name == "Scene");

            New(sceneType);
        }

        private void OpenSceneDocument()
        {
            var openFileDialog = new OpenFileDialog()
            {
                Title = "Open File",
                CheckFileExists = true,
                CheckPathExists = true,
                Multiselect = false,
                RestoreDirectory = true,
                ValidateNames = true
            };

            openFileDialog.Filter = "fbx files (*.fbx)|*.fbx|x files (*.x)|*.x|All files (*.*)|*.*";
            openFileDialog.FilterIndex = 1;

            bool? result = openFileDialog.ShowDialog();
            if (result != true)
            {
                return;
            }

            Open(openFileDialog.FileName);
        }

        private void AddMenus()
        {
            var insertBeforeCloseSeparator = new MergePoint(MergeOperation.InsertBefore, "CloseSeparator");
            var insertBeforeSaveSeparator = new MergePoint(MergeOperation.InsertBefore, "SaveSeparator");

            m_menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("FileGroup", "File"), insertBeforeCloseSeparator,
                    new TreeNode<ICommandItem>(new CommandGroup("FileNewGroup", "New"), insertBeforeCloseSeparator,
                        new TreeNode<ICommandItem>(CommandItems["Scene"])),

                    new TreeNode<ICommandItem>(CommandItems["Open"], insertBeforeCloseSeparator),
                    new TreeNode<ICommandItem>(CommandItems["Close"], insertBeforeCloseSeparator),
                    new TreeNode<ICommandItem>(CommandItems["CloseAll"], insertBeforeCloseSeparator),

                    new TreeNode<ICommandItem>(CommandItems["Save"], insertBeforeSaveSeparator),
                    new TreeNode<ICommandItem>(CommandItems["SaveAll"], insertBeforeSaveSeparator))
            };

            Editor.MenuNodes.Add(m_menuNodes);
        }

        protected override void OnUninitialize()
        {
            Editor.ActiveItemChanged -= OnEditorActiveItemChanged;

            RemoveMenus();
            RemoveCommands();
        }

        private void RemoveMenus()
        {
            Editor.MenuNodes.Remove(m_menuNodes);
            m_menuNodes.Clear();
        }

        private void RemoveCommands()
        {
            CommandItems.Clear();
        }

        protected override void OnShutdown()
        {
            Editor.Services.UnregisterHandler(typeof(IDocumentService));
        }

        public Document New(DocumentType documentType)
        {
            var selectedFactory = m_factories.FirstOrDefault(factory => factory.SupportedFileTypes.Contains(documentType));
            if (selectedFactory == default(DocumentFactory))
            {
            }

            var document = selectedFactory.Create(documentType);
            if (document != null)
            {
                document.New();
                var viewModel = document.CreateViewModel();

                m_contentBrowserService.Load(string.Empty);
                Editor.ActivateItem(viewModel);
            }

            return document;
        }

        public bool CanOpen(string filename)
        {
            var fileExtension = Path.GetExtension(filename);
            return m_factories.SelectMany(factory => factory.SupportedFileTypes)
                .Any(documentType => documentType.FileExtensions.Contains(fileExtension));
        }

        public Document Open(string filename)
        {
            for (int i = 0; i < m_documents.Count; i++)
            {
                var existingDocument = m_documents[i];
                if (existingDocument.FilePath == filename)
                {
                    ShowDocument(existingDocument);
                    return existingDocument;
                }
            }

            DocumentViewModel documentViewModel = null;
            Document existingBlankDocument = null;
            for (int i = 0; i < m_documents.Count; i++)
            {
                if (!m_documents[i].IsModified && m_documents[i].IsUntitled)
                {
                    existingBlankDocument = m_documents[i];
                    documentViewModel = m_documents[i].ViewModel;
                    break;
                }
            }

            var fileExtension = Path.GetExtension(filename).ToLower();
            var selectedDocumentType = m_factories.SelectMany(factory => factory.SupportedFileTypes)
                .FirstOrDefault(fileType => fileType.FileExtensions.Contains(fileExtension));

            var newDocument = selectedDocumentType.Factory.Create(selectedDocumentType, existingBlankDocument);
            newDocument.Load(filename);

            if (documentViewModel == null)
            {
                documentViewModel = newDocument.CreateViewModel();
            }

            m_contentBrowserService.Load(Path.GetDirectoryName(filename));
            Editor.ActivateItem(documentViewModel);

            return newDocument;
        }

        private void ShowDocument(Document existingDocument)
        {
            var viewModel = existingDocument.ViewModel;

            Editor.ActivateItem(viewModel);
        }

        public bool Close(Document document, bool force)
        {
            if (document.IsDisposed)
                return true;

            bool shouldClose = force || ShowSaveChangesDialog(document);
            if (shouldClose)
            {
                document.Dispose();

                return true;
            }

            return false;
        }

        public bool ShowSaveChangesDialog(Document document)
        {
            if (!document.IsModified)
                return true;

            var saveChangesDialog = new SaveChangesViewModel
            {
                ModifiedDocuments = new[] { document }
            };
            _windowManager.ShowDialog(saveChangesDialog);

            if (saveChangesDialog.SaveChangesDialogResult == SaveChangesDialogResult.SaveAndClose)
            {
                Save(document);
                return true;
            }

            if (saveChangesDialog.SaveChangesDialogResult == SaveChangesDialogResult.CloseWithoutSaving)
            {
                return true;
            }

            return false;
        }

        private void Save(Document document)
        {
            document.Save();
        }

        public async Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            return await CloseAllDocumentsAsync();
        }

        private async Task<bool> CloseAllDocumentsAsync()
        {
            var modifiedDocuments = m_documents.Where(document => document.IsModified && !document.IsDisposed).ToArray();

            if (modifiedDocuments.Length > 0)
            {
                var saveChangesDialog = new SaveChangesViewModel
                {
                    ModifiedDocuments = modifiedDocuments
                };
                _ = _windowManager.ShowDialog(saveChangesDialog);

                if (saveChangesDialog.SaveChangesDialogResult == SaveChangesDialogResult.SaveAndClose)
                {
                    SaveAll(modifiedDocuments);
                }
                else if (saveChangesDialog.SaveChangesDialogResult == SaveChangesDialogResult.CloseWithoutSaving)
                {
                }
                else
                {
                    return false;
                }
            }

            var copiedDocuments = m_documents.ToArray();
            foreach (var document in copiedDocuments)
            {
                Close(document, true);
                await Dispatcher.Yield();
            }
            return true;
        }

        private void SaveAll(IEnumerable<Document> modifiedDocuments)
        {
            foreach (var document in modifiedDocuments)
            {
                document.Save();
            }
        }

        public void RecordDocument(Document document)
        {
            m_documents.Add(document);
        }

        public void UnrecordDocument(Document document)
        {
            m_documents.Remove(document);

            if (ActiveDocument == document)
            {
                ActiveDocument = null;
            }
        }
    }
}
