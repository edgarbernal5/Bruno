
using Estero.Collections;
using EsteroFramework.Graphics;
using Microsoft.Win32;
using System;
using System.IO;
using TrioApi.Net.Content;
using TrioApi.Net.Content.Tasks;
using TrioApi.Net.Game;
using TrioApi.Net.Graphics;

namespace EsteroFramework.Editor.Units
{
    public class BuilderUnit : EditorUnit
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;
        private OpenFileDialog m_openFileDialog;

        public BuilderUnit()
        {
        }

        protected override void OnInitialize()
        {
            m_openFileDialog = new OpenFileDialog()
            {
                Title = "Open File",
                CheckFileExists = true,
                CheckPathExists = true,
                Multiselect = false,
                RestoreDirectory = true,
                ValidateNames = true
            };
        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("Build", new DelegateCommand(() => Build()))
                {
                    Text = "Build",
                }
            );
        }

        private void Build()
        {
            m_openFileDialog.Filter = "x files (*.x)|*.x|fbx files (*.fbx)|*.fbx|All files (*.*)|*.*";
            m_openFileDialog.FilterIndex = 1;

            bool? result = m_openFileDialog.ShowDialog();
            if (result != true)
                return;

            var filename = m_openFileDialog.FileName;

            BuildCoordinatorSettings settings = new BuildCoordinatorSettings();
            BuildCoordinator buildCoordinator = new BuildCoordinator(settings);

            settings = buildCoordinator.Settings;

            buildCoordinator.RequestBuild(filename, Path.GetFileName(filename), "ModelImporter", "ModelProcessor");
            buildCoordinator.RunTheBuild();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            ContentManager contentManager = new ContentManager(graphicsDevice, settings.OutputDirectory);

            var modelPath = Path.GetFileName(filename);
            Model model = contentManager.Load<Model>(modelPath);

            string basePath = @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\TrioEngine\TrioWpfEditor\bin\x64\Debug\finalObjects\Car.fbx.estero";
            string path = @"D:\Edgar\Documentos\Proyectos\CG\TrioEngineGit\TrioEngine\TrioWpfEditor\bin\x64\Debug\finalObjects\RacerCar_0";

            string finale = GetRelativePath(basePath, path);

            Scene.ActiveScene.LoadFromModel(model);

            buildCoordinator.Dispose();
        }

        public string GetRelativePath(string basePath, string path)
        {
            Uri uri;
            if (!Uri.TryCreate(path, UriKind.Absolute, out uri))
                return path;

            uri = new Uri(basePath).MakeRelativeUri(uri);
            var str = Uri.UnescapeDataString(uri.ToString());

            return Normalize(str);
        }
        public string Normalize(string path)
        {
            return path.Replace('\\', '/');
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("FileGroup", "File"),
                    new TreeNode<ICommandItem>(CommandItems["Build"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnShutdown()
        {
            RemoveMenus();
            RemoveCommands();
        }

        private void RemoveMenus()
        {
            Editor.MenuNodes.Remove(_menuNodes);
            _menuNodes.Clear();
        }

        private void RemoveCommands()
        {
            CommandItems.Clear();
        }

        protected override void OnUninitialize()
        {

        }
    }
}
