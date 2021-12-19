
using Bruno.Collections;
using Microsoft.Win32;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace BrunoFramework.Editor.Units
{
    public class DocumentUnit : EditorUnit, IDocumentService
    {
        private TreeNodeCollection<ICommandItem> m_menuNodes;

        public List<DocumentFactory> Factories { get => m_factories; }
        private List<DocumentFactory> m_factories;

        private List<Document> m_documents;

        public DocumentUnit()
        {
            m_factories = new List<DocumentFactory>();
            m_documents = new List<Document>();
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IDocumentService), null, this);

            m_factories.Add(new SceneDocumentFactory(Editor));
        }

        protected override void OnStartup()
        {
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
        }

        private void NewSceneDocument()
        {

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

            openFileDialog.Filter = "x files (*.x)|*.x|fbx files (*.fbx)|*.fbx|All files (*.*)|*.*";
            openFileDialog.FilterIndex = 1;

            bool? result = openFileDialog.ShowDialog();
            if (result != true)
                return;

            Open(openFileDialog.FileName);
        }

        private void AddMenus()
        {
            m_menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("FileGroup", "File"),
                    new TreeNode<ICommandItem>(new CommandGroup("FileNewGroup", "New"),
                    new TreeNode<ICommandItem>(CommandItems["Scene"])),
                    new TreeNode<ICommandItem>(CommandItems["Open"]))
            };

            Editor.MenuNodes.Add(m_menuNodes);
        }

        protected override void OnUninitialize()
        {
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
                Editor.ActivateItem(viewModel);
            }

            return document;
        }

        public Document Open(string filename)
        {
            for (int i = 0; i < m_documents.Count; i++)
            {
                var existingDocument = m_documents[i];
                if (existingDocument.FilenamePath == filename)
                {

                    return existingDocument;
                }

            }

            var fileExtension = Path.GetExtension(filename).ToLower();
            var selectedDocumentType = m_factories.SelectMany(factory => factory.SupportedFileTypes)
                .FirstOrDefault(fileType => fileType.FileExtensions.Contains(fileExtension));
            var selectedFactory = selectedDocumentType.Factory;

            var document = selectedFactory.Create(selectedDocumentType);

            document.Load(filename);
            var viewModel = document.CreateViewModel();
            Editor.ActivateItem(viewModel);

            m_documents.Add(document);

            return document;
        }

        public override EditorDockableTabViewModel GetDockTabViewModel(string dockId)
        {
            return null;
        }
    }
}
