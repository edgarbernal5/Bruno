
using Bruno.Collections;
using BrunoFramework.Graphics;
using Microsoft.Win32;
using BrunoApi.Net.Content;
using BrunoApi.Net.Content.Tasks;
using BrunoApi.Net.Game;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Editor.Units
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
            //AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("Build", new DelegateCommand(() => Build()))
                {
                    Text = "Build",
                }
            );

            CommandItems.Add(
                new CommandItem("Box", new DelegateCommand(() => Box()))
                {
                    Text = "Box",
                }
            );
        }

        private void Box()
        {
            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;

            VertexDeclaration vertexDeclaration = VertexPositionNormalTexture.VertexDeclaration;
            VertexBuffer vertexBuffer = new VertexBuffer(graphicsDevice, vertexDeclaration, 4);
            VertexPositionNormalTexture[] vertices = new VertexPositionNormalTexture[]
            { 
                new VertexPositionNormalTexture(new Vector3(5, 0, 5), Vector3.Up, new Vector2(1, 1)),
                new VertexPositionNormalTexture(new Vector3(-5, 0, 5), Vector3.Up, new Vector2(0, 1) ),
                new VertexPositionNormalTexture(new Vector3(-5, 0, -5), Vector3.Up,  Vector2.Zero),
                new VertexPositionNormalTexture(new Vector3(5, 0, -5), Vector3.Up,  new Vector2(1, 0))
            };
            vertexBuffer.SetData(vertices);

            IndexBuffer indexBuffer = new IndexBuffer(graphicsDevice, IndexElementSize.SixteenBits, 6);
            short[] indices = new short[] 
            {
                0, 1, 2,
                3, 0, 2
            };

            indexBuffer.SetData(indices);

            Material material = new Material("material");
            Texture2D texture = new Texture2D(graphicsDevice, @"D:\Edgar\Documentos\Proyectos\CG\BrunoEngineGit\Models\Car\RacerCar.png");
            material.InsertTexture("Texture", texture);

            Model model = new Model(graphicsDevice, vertexBuffer, indexBuffer, material);
            Scene.ActiveScene.LoadFromModel(model);
        }

        private void Build()
        {
            m_openFileDialog.Filter = "x files (*.x)|*.x|fbx files (*.fbx)|*.fbx|All files (*.*)|*.*";
            m_openFileDialog.FilterIndex = 1;

            bool? result = m_openFileDialog.ShowDialog();
            if (result != true)
                return;

            var filename = m_openFileDialog.FileName;

            GameContentBuilderSettings settings = new GameContentBuilderSettings();
            GameContentBuilder buildCoordinator = new GameContentBuilder(settings);

            settings = buildCoordinator.Settings;

            var assetNamePath = buildCoordinator.GetRelativePath(filename); //Path.GetFileName(filename);

            buildCoordinator.RequestBuild(filename, assetNamePath, "ModelImporter", "ModelProcessor");
            buildCoordinator.RunTheBuild();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            var contentManager = new ContentManager(graphicsDevice, settings.OutputDirectory);

            var model = contentManager.Load<Model>(assetNamePath);

            Scene.ActiveScene.LoadFromModel(model);

            var outputFiles = buildCoordinator.GetOutputFiles();
            buildCoordinator.Dispose();
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
                    new TreeNode<ICommandItem>(CommandItems["Build"]),
                    new TreeNode<ICommandItem>(CommandItems["Box"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnShutdown()
        {
            //RemoveMenus();
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
