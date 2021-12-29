
using BrunoFramework.Editor.Units;
using BrunoFramework.Graphics;
using System.IO;
using BrunoApi.Net.Content;
using BrunoApi.Net.Content.Tasks;
using BrunoApi.Net.Game;
using BrunoApi.Net.Graphics;
using System.Linq;
using System.Collections.Specialized;
using BrunoFramework.Editor.Game.Gizmos;
using BrunoFramework.Editor.Timing;
using BrunoFramework.Editor.Game.Inspectors;
using System.ComponentModel;
using System;

namespace BrunoFramework.Editor.Game
{
    public class SceneDocument : Document
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
        private ContentBrowser m_contentBrowser;
        private IWorldOutlineService m_outlineService;
        private IInspectorService m_inspectorService;
        private IContentBrowserService m_contentBrowserService;

        public SceneDocument(IEditorService editor, DocumentType documentFileType) 
            : base(editor, documentFileType)
        {
            m_outlineService = Editor.Services.GetInstance<IWorldOutlineService>();
            m_inspectorService = Editor.Services.GetInstance<IInspectorService>();
            m_contentBrowserService = Editor.Services.GetInstance<IContentBrowserService>();

            m_worldOutline = new WorldOutline();
            var objectSelector = new ObjectSelector(m_worldOutline);

            m_contentBrowser = new ContentBrowser();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            var gizmoService = new GizmoService(graphicsDevice, objectSelector, Editor.Services.GetInstance<GameStepTimer>());
            gizmoService.CurrentGizmo = GizmoType.Translation;
            GizmoService = gizmoService;

            editor.ActiveDocumentChanged += Editor_ActiveDocumentChanged;
        }

        private void Editor_ActiveDocumentChanged(object sender, EventArgs eventArgs)
        {

        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                Editor.ActiveDocumentChanged -= Editor_ActiveDocumentChanged;
            }
        }

        private void UpdateWorldOutline()
        {
            if (m_worldOutline != null)
            {
                m_worldOutline.SelectedItems.CollectionChanged -= OnWorldOutlineSelectionChanged;
                m_worldOutline.Dispose();

                m_worldOutline.RootItems.Clear();
                m_worldOutline.SelectedItems.Clear();
            }
            
            PopulateHierarchyFromScene();
            m_worldOutline.SelectedItems.CollectionChanged += OnWorldOutlineSelectionChanged;

            //TODO: hacer esto si y solo si es el documento activo (pestaña)
            m_outlineService.WorldOutline = m_worldOutline;
        }

        private void UpdateContentBrowser(string rootDirectory)
        {
            if (m_contentBrowser != null)
            {
                m_contentBrowser.Dispose();
                m_contentBrowser.Clear();
            }
            PopulateContentBrowser(rootDirectory);

            //TODO: hacer esto si y solo si es el documento activo (pestaña) (ver si aplica)
            m_contentBrowserService.ContentBrowser = m_contentBrowser;
        }

        private void PopulateContentBrowser(string rootDirectory)
        {
            if (string.IsNullOrEmpty(rootDirectory))
            {
                return;
            }

            var populate = new ContentBrowserPopulate(m_contentBrowser, new FileSystemStorage(rootDirectory));
            populate.Execute();
        }

        private void OnWorldOutlineSelectionChanged(object sender, NotifyCollectionChangedEventArgs eventArgs)
        {
            if (m_worldOutline == null || m_inspectorService == null)
                return;

            if (m_outlineService.WorldOutline != m_worldOutline)
                return;

            m_gizmoService.IsActive = m_worldOutline.SelectedItems.Count > 0;
            if (m_worldOutline.SelectedItems.Count > 0)
            {
                var entityId = m_worldOutline.SelectedItems[0].Id;
                var editorObject = m_worldOutline.SelectedItems[0].CustomData as IEditorObject;

                //bool savedNotifiying = customData.IsNotifying;
                //customData.IsNotifying = false;
                SetSceneTransformFor(entityId, editorObject);

                //customData.IsNotifying = savedNotifiying;

                m_gizmoService.SetTransformableAsSelected(editorObject);

                m_inspectorService.SelectedObject = new InspectableObjectBuilder()
                    .WithObjectProperties(editorObject, x => true)
                    .ToInspectableObject();
            }
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

            foreach (var itemInHierarchy in groupByParent)
            {
                if (itemInHierarchy.Parent == 0)
                {
                    foreach (var entity in itemInHierarchy.Items)
                    {
                        var outlineItem = CreateOutlineItem(entity.id, itemInHierarchy.Parent, entity.name);
                        CreateAndSetCustomData(entity.name, outlineItem, entity);

                        m_worldOutline.RootItems.Add(outlineItem);
                    }
                }
                else
                {
                    foreach (var entity in itemInHierarchy.Items)
                    {
                        for (int i = 0; i < m_worldOutline.RootItems.Count; i++)
                        {
                            var container = FindParent(m_worldOutline.RootItems[i], itemInHierarchy.Parent);
                            if (container != null)
                            {
                                var outlineItem = CreateOutlineItem(entity.id, itemInHierarchy.Parent, entity.name);
                                CreateAndSetCustomData(entity.name, outlineItem, entity);

                                container.Children.Add(outlineItem);
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

        private void SetSceneTransformFor(long entityId, IEditorObject editorObject)
        {
            var sceneTransform = m_scene.GetSceneTransformFor(entityId);
            editorObject.LocalPosition = sceneTransform.LocalPosition;
            editorObject.LocalRotation = sceneTransform.LocalRotation;
            editorObject.LocalScale = sceneTransform.LocalScale;
            editorObject.WorldMatrix = sceneTransform.WorldMatrix;
        }

        private void CreateAndSetCustomData(string name, WorldOutlineItem outlineItem, Scene.HierarchyComponentBridge entity)
        {
            var customData = new WorldOutlineData();
            customData.Name = name;
            customData.Owner = outlineItem;

            SetSceneTransformFor(entity.id, customData);

            customData.PropertyChanged += CustomData_PropertyChanged;

            if ((entity.componentsMask & 16) != 0)
            {
                var boundingBoxExtents = m_scene.GetBoundingBoxForEntity(entity.id);
                customData.SetObjectExtents(boundingBoxExtents.Center, boundingBoxExtents.Extents);
            }

            outlineItem.CustomData = customData;
        }

        private void CustomData_PropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalPosition))
            {
                var customData = sender as WorldOutlineData;

                m_scene.TransformSetLocalPosition(customData.Owner.Id, customData.LocalPosition);
                m_scene.Update(); //hacer un update de solo los componentes involucrados?

                if (m_worldOutline.SelectedItems.Count > 0)
                {
                    var id = m_worldOutline.SelectedItems[0].Id;
                    var sceneTransform = m_scene.GetSceneTransformFor(id);

                    customData.WorldMatrix = sceneTransform.WorldMatrix;
                    m_gizmoService.SetTransformableAsSelected(customData);
                }
            }
            else if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalScale))
            {
                var customData = sender as WorldOutlineData;
                m_scene.TransformSetLocalScale(customData.Owner.Id, customData.LocalScale);
            }
            else if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalRotation))
            {
                var customData = sender as WorldOutlineData;

                m_scene.TransformSetLocalRotation(customData.Owner.Id, customData.LocalRotation);
                m_scene.Update();

                var sceneTransform = m_scene.GetSceneTransformFor(customData.Owner.Id);
                customData.WorldMatrix = sceneTransform.WorldMatrix;
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
            UpdateContentBrowser(string.Empty);
        }

        protected override void OnLoad(string filename)
        {
            var settings = new BuildCoordinatorSettings();
            settings.RootDirectory = Path.GetDirectoryName(filename);

            var buildCoordinator = new BuildCoordinator(settings);

            settings = buildCoordinator.Settings;

            var assetNamePath = buildCoordinator.GetRelativePath(filename);

            buildCoordinator.RequestBuild(filename, assetNamePath, "ModelImporter", "ModelProcessor");
            buildCoordinator.RunTheBuild();
            buildCoordinator.Dispose();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            ContentManager contentManager = new ContentManager(graphicsDevice, settings.OutputDirectory);

            var model = contentManager.Load<Model>(assetNamePath);

            Scene = new Scene();
            m_scene.LoadFromModel(model);

            UpdateWorldOutline();
            UpdateContentBrowser(settings.RootDirectory);
        }

        protected override void OnSave()
        {

        }

        public override DocumentViewModel CreateViewModel()
        {
            return new SceneDocumentViewModel(this);
        }
    }
}
