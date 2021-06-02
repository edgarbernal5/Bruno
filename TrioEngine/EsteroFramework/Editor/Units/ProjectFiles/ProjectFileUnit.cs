
using Estero.Collections;
using Microsoft.Win32;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace EsteroFramework.Editor.Units
{
    public class ProjectFileUnit : EditorUnit, IProjectFileService
    {
        private TreeNodeCollection<ICommandItem> m_menuNodes;

        public List<ProjectFileFactory> Factories { get => m_factories; }
        private List<ProjectFileFactory> m_factories;

        public ProjectFileUnit()
        {
            m_factories = new List<ProjectFileFactory>();
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IProjectFileService), null, this);

            m_factories.Add(new SceneProjectFileFactory(Editor));
        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("Scene", new DelegateCommand(() => NewProjectFile()))
                {
                    Text = "Scene",
                }
            );
            CommandItems.Add(
                new CommandItem("Open", new DelegateCommand(() => OpenProjectFile()))
                {
                    Text = "Open",
                }
            );
        }

        private void NewProjectFile()
        {

        }

        private void OpenProjectFile()
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
            Editor.Services.UnregisterHandler(typeof(IProjectFileService));
        }

        public ProjectFile New(ProjectFileType projectFileType)
        {
            var selectedFactory = m_factories.FirstOrDefault(factory => factory.SupportedFileTypes.Contains(projectFileType));
            if (selectedFactory == default(ProjectFileFactory))
            {

            }

            var projectFile = selectedFactory.Create(projectFileType);
            if (projectFile != null)
            {
                projectFile.New();
                var viewModel = projectFile.CreateViewModel();
                Editor.ActivateItem(viewModel);
            }

            return projectFile;
        }

        public ProjectFile Open(string filename)
        {
            var fileExtension = Path.GetExtension(filename).ToLower();
            var selectedFileType = m_factories.SelectMany(factory => factory.SupportedFileTypes)
                .FirstOrDefault(fileType => fileType.FileExtensions.Contains(fileExtension));
            var selectedFactory = selectedFileType.Factory;

            var projectFile = selectedFactory.Create(selectedFileType);

            projectFile.Load(filename);
            var viewModel = projectFile.CreateViewModel();
            Editor.ActivateItem(viewModel);

            return projectFile;
        }
    }
}
