
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics;
using System.IO;
using TrioApi.Net.Content;
using TrioApi.Net.Content.Tasks;
using TrioApi.Net.Game;
using TrioApi.Net.Graphics;
using System.Linq;
using System.Collections.Specialized;
using EsteroFramework.Editor.Game.Gizmos;
using TrioApi.Net.Maths;

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

        public GizmoService GizmoService
        {
            get => m_gizmoService;
            set
            {
                m_gizmoService = value;
                NotifyOfPropertyChange();
            }
        }
        private GizmoService m_gizmoService;

        private WorldOutline m_worldOutline;
        private IWorldOutlineService m_outlineService;

        public SceneProjectFile(IEditorService editor) :
            base(editor)
        {
            m_outlineService = Editor.Services.GetInstance<IWorldOutlineService>();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            GizmoService = new GizmoService(graphicsDevice);
        }

        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

        private void UpdateWorldOutline()
        {
            if (m_worldOutline != null)
            {
                m_worldOutline.SelectedItems.CollectionChanged -= OnWorldOutlineSelectionChanged;
                m_worldOutline.Dispose();
            }
            m_worldOutline = new WorldOutline();
            PopulateHierarchyFromScene();
            m_worldOutline.SelectedItems.CollectionChanged += OnWorldOutlineSelectionChanged;

            m_gizmoService.OnTranslationChanged += OnGizmoTranslationChanged;

            //TODO: hacer esto si y solo si es el documento activo (pestaña)
            m_outlineService.WorldOutline = m_worldOutline;
        }

        private void OnWorldOutlineSelectionChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            if (m_worldOutline == null)
                return;

            if (m_outlineService.WorldOutline != m_worldOutline)
                return;

            m_gizmoService.IsActive = m_worldOutline.SelectedItems.Count > 0;
            if (m_worldOutline.SelectedItems.Count > 0)
            {
                var id = m_worldOutline.SelectedItems[0].Id;
                var sceneTransform = m_scene.GetSceneTransformFor(id);

                m_gizmoService.SetObjectSelected(new GizmoTransformable(id, sceneTransform));
            }
        }

        private void OnGizmoTranslationChanged(GizmoTransformable gizmoTransformable, Vector3 delta)
        {
            m_scene.TransformTranslate(gizmoTransformable.Id, delta);

            //var sceneTransform = m_scene.GetSceneTransformFor(gizmoTransformable.Id);
            //m_gizmoService.SetObjectSelected(new GizmoTransformable(gizmoTransformable.Id, sceneTransform));
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
                            else
                            {
                                throw new System.Exception();
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

                var parentResult = FindParent(item, parentId);
                if (parentResult != null)
                    return parentResult;
            }

            return null;
        }

        protected override void OnNew()
        {
            Scene = new Scene();
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

            Scene = new Scene();
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
