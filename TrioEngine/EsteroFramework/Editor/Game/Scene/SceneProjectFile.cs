
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics;
using System.IO;
using TrioApi.Net.Content;
using TrioApi.Net.Content.Tasks;
using TrioApi.Net.Game;
using TrioApi.Net.Graphics;
using System.Linq;

namespace EsteroFramework.Editor.Game
{
    public class SceneProjectFile : ProjectFile
    {
        public Scene Scene
        {
            get => m_scene;
            set
            {
                m_scene = value;
                NotifyOfPropertyChange();
            }
        }
        private Scene m_scene;

        private WorldOutline m_worldOutline;

        public SceneProjectFile(IEditorService editor) :
            base(editor)
        {
        }

        private void UpdateWorldOutline()
        {
            if (m_worldOutline != null)
            {
                m_worldOutline.Dispose();
            }
            m_worldOutline = new WorldOutline();
            PopulateHierarchyFromScene();

            var outlineService = Editor.Services.GetInstance<IWorldOutlineService>();
            outlineService.WorldOutline = m_worldOutline;
        }

        private WorldOutlineItem CreateOutlineItem(long entity, long parentId, string name)
        {
            return new WorldOutlineItem()
            {
                Text = name,
                Id = entity,
                ParentId = parentId,
                Children = new WorldOutlineItemCollection()
            };
        }

        private void PopulateHierarchyFromScene()
        {
            var sceneHierarchy = m_scene.GetHierarchies();
            var groupByParent = sceneHierarchy.GroupBy(x => x.parentId).Select(x => new { Parent = x.Key, Items = x.ToList() }).ToList();

            foreach (var item in groupByParent)
            {
                if (item.Parent == 0)
                {
                    foreach (var entityName in item.Items)
                    {
                        m_worldOutline.RootItems.Add(CreateOutlineItem(entityName.id, item.Parent, entityName.name));
                    }
                }
                else
                {
                    foreach (var entityName in item.Items)
                    {
                        for (int i = 0; i < m_worldOutline.RootItems.Count; i++)
                        {
                            var container = FindParent(m_worldOutline.RootItems[i], item.Parent);
                            if (container != null)
                            {
                                container.Children.Add(CreateOutlineItem(entityName.id, item.Parent, entityName.name));
                                break;
                            }
                        }
                    }
                }
            }
        }

        private WorldOutlineItem FindParent(WorldOutlineItem currentContainer, long parentId)
        {
            if (currentContainer.Id == parentId)
                return currentContainer;

            foreach (var item in currentContainer.Children)
            {
                if (item.Id == parentId)
                    return item;

                var ret = FindParent(item, parentId);
                if (ret != null)
                    return ret;
            }

            return null;
        }

        protected override void OnNew()
        {
            m_scene = new Scene();
            UpdateWorldOutline();
        }

        protected override void OnLoad(string filename)
        {
            BuildCoordinatorSettings settings = new BuildCoordinatorSettings();
            settings.RootDirectory = Path.GetDirectoryName(filename);

            BuildCoordinator buildCoordinator = new BuildCoordinator(settings);

            settings = buildCoordinator.Settings;

            var assetNamePath = buildCoordinator.GetRelativePath(filename);

            buildCoordinator.RequestBuild(filename, assetNamePath, "ModelImporter", "ModelProcessor");
            buildCoordinator.RunTheBuild();
            buildCoordinator.Dispose();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            ContentManager contentManager = new ContentManager(graphicsDevice, settings.OutputDirectory);

            Model model = contentManager.Load<Model>(assetNamePath);

            m_scene = new Scene();
            m_scene.LoadFromModel(model);

            UpdateWorldOutline();
        }

        protected override void OnSave()
        {

        }

        public override ProjectFileViewModel CreateViewModel()
        {
            return new SceneViewModel(this);
        }
    }
}
