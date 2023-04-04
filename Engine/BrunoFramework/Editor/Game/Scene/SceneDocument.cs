
using BrunoFramework.Editor.Units;
using BrunoFramework.Graphics;
using System.IO;
using BrunoApi.Net.Content;
using BrunoApi.Net.Content.Tasks;
using BrunoApi.Net.Game;
using BrunoApi.Net.Graphics;
using System.Linq;
using BrunoFramework.Editor.Game.Gizmos;
using BrunoFramework.Editor.Timing;
using BrunoFramework.Editor.Game.Properties;
using System.ComponentModel;
using System;
using BrunoApi.Net.IO;
using BrunoFramework.Graphics.Data;
using System.Collections.Generic;
using Bruno.Linq;

namespace BrunoFramework.Editor.Game
{
    public class SceneDocument : Document, IWorldOutlineHandler
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

        public IGizmoService GizmoService
        {
            get => m_gizmoService;
            set
            {
                m_gizmoService = value;
                NotifyOfPropertyChange();
            }
        }
        private IGizmoService m_gizmoService;

        public Camera Camera
        {
            get => m_camera;
            set
            {
                m_camera = value;
                m_gizmoService.Camera = m_camera;
                NotifyOfPropertyChange();
            }
        }
        private Camera m_camera;

        public WorldOutline WorldOutline => m_worldOutline;
        private WorldOutline m_worldOutline;

        private IWorldOutlineService m_outlineService;
        private IPropertiesService m_propertiesService;
        private IDocumentService m_documentService;

        public SceneDocument(IEditorService editor, DocumentType documentFileType)
            : base(editor, documentFileType)
        {
            m_outlineService = Editor.Services.GetInstance<IWorldOutlineService>();
            m_propertiesService = Editor.Services.GetInstance<IPropertiesService>();
            m_documentService = Editor.Services.GetInstance<IDocumentService>();

            m_worldOutline = new WorldOutline(this);

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            var objectSelector = new ObjectSelector(m_worldOutline);
            var gizmoService = new GizmoService(graphicsDevice, objectSelector, Editor.Services.GetInstance<GameStepTimer>());
            gizmoService.CurrentGizmo = GizmoType.Translation;
            GizmoService = gizmoService;

            editor.ActiveItemChanged += OnEditorActiveItemChanged;
        }

        private void OnEditorActiveItemChanged(object sender, EventArgs eventArgs)
        {
            if (m_documentService.ActiveDocument == this)
            {
                if (m_outlineService != null)
                {
                    m_outlineService.WorldOutline = m_worldOutline;

                    UpdateProperties();
                }
            }
        }


        public void DeleteObjects(List<long> entities)
        {
            if (entities.Count == 0) return;

            var items = m_worldOutline.RootItems.Concat(m_worldOutline.RootItems.SelectMany(x => x.GetDescendants((y) => y.Children, true)))
                .Where(item => entities.Contains(item.Id)).ToList();

            foreach (var item in items)
            {
                m_worldOutline.RemoveItem(item);
                m_scene.RemoveEntity(item.Id);
            }

            IsModified = true;
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                m_worldOutline.RootItems.Clear();
                m_worldOutline.SelectedItems.Clear();

                m_worldOutline.Dispose();

                Editor.ActiveItemChanged -= OnEditorActiveItemChanged;
            }
            base.OnDisposing(disposing);
        }

        private void UpdateWorldOutline()
        {
            DisposeWorldOutline();

            PopulateHierarchyFromScene();

            if (m_documentService.ActiveDocument == this)
            {
                m_outlineService.WorldOutline = m_worldOutline;
            }
        }

        private void DisposeWorldOutline()
        {
            if (m_worldOutline != null)
            {
                //m_worldOutline.SelectedItems.Clear();
                m_worldOutline.RootItems.Clear();
            }
        }

        public void SelectionChanged()
        {
            if (m_worldOutline == null || m_propertiesService == null)
                return;

            if (m_outlineService.WorldOutline != m_worldOutline)
                return;

            m_gizmoService.IsActive = m_worldOutline.SelectedItems.Count > 0;
            if (m_worldOutline.SelectedItems.Count == 0)
            {
                m_propertiesService.SelectedObject = PropertiesObjectBuilder.Empty;
            }
            else
            {
                var entityId = m_worldOutline.SelectedItems[0].Id;
                var editorObject = m_worldOutline.SelectedItems[0].CustomData as IEditorObject;

                var customData = m_worldOutline.SelectedItems[0].CustomData as WorldOutlineData;
                bool savedNotifiying = customData.IsNotifying;
                customData.IsNotifying = false;
                SetSceneTransformFor(entityId, editorObject);

                customData.IsNotifying = savedNotifiying;

                m_gizmoService.SetEditorObjectAsSelected(editorObject);

                m_propertiesService.SelectedObject = new PropertiesObjectBuilder()
                    .WithObjectProperties(editorObject, x => true)
                    .ToPropertiesObject();
            }
        }

        public void AddEmptyObject(string name)
        {
            var entity = Scene.AddEmptyObject(name);
            var itemInHierarchy = Scene.GetHierarchyForEntity(entity);

            var outlineItem = CreateOutlineItem(entity, itemInHierarchy.parentId, name);
            CreateAndSetCustomData(name, outlineItem, itemInHierarchy);

            m_worldOutline.RootItems.Add(outlineItem);
            IsModified = true;
        }

        private void UpdateProperties()
        {
            if (m_worldOutline.SelectedItems.Count == 0)
            {
                m_propertiesService.SelectedObject = PropertiesObjectBuilder.Empty;
            }
            else
            {
                var editorObject = m_worldOutline.SelectedItems[0].CustomData as IEditorObject;

                m_propertiesService.SelectedObject = new PropertiesObjectBuilder()
                    .WithObjectProperties(editorObject, x => true)
                    .ToPropertiesObject();
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
            var customData = new WorldOutlineData(outlineItem);
            customData.Name = name;

            SetSceneTransformFor(entity.id, customData);

            customData.PropertyChanged += OnCustomDataPropertyChanged;

            if ((entity.componentsMask & 16) != 0)
            {
                var boundingBox = m_scene.GetBoundingBoxForEntity(entity.id);

                customData.SetObjectExtents(boundingBox.Center, boundingBox.Extents);
            }

            outlineItem.CustomData = customData;
        }

        private void OnCustomDataPropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalPosition))
            {
                var customData = sender as WorldOutlineData;

                m_scene.TransformSetLocalPosition(customData.Owner.Id, customData.LocalPosition);
                m_scene.TransformUpdate(customData.Owner.Id);
                //m_scene.Update();

                IsModified = true;
                if (m_worldOutline.SelectedItems.Count > 0)
                {
                    var sceneTransform = m_scene.GetSceneTransformFor(customData.Owner.Id);

                    customData.WorldMatrix = sceneTransform.WorldMatrix;
                    m_gizmoService.UpdateSelectedObject(customData);
                }
            }
            else if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalScale))
            {
                var customData = sender as WorldOutlineData;
                m_scene.TransformSetLocalScale(customData.Owner.Id, customData.LocalScale);
                m_scene.TransformUpdate(customData.Owner.Id);

                IsModified = true;
                if (m_worldOutline.SelectedItems.Count > 0)
                {
                    var sceneTransform = m_scene.GetSceneTransformFor(customData.Owner.Id);

                    customData.WorldMatrix = sceneTransform.WorldMatrix;
                    m_gizmoService.UpdateSelectedObject(customData);
                }
            }
            else if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalRotation))
            {
                var customData = sender as WorldOutlineData;

                m_scene.TransformSetLocalRotation(customData.Owner.Id, customData.LocalRotation);
                m_scene.TransformUpdate(customData.Owner.Id);

                IsModified = true;
                if (m_worldOutline.SelectedItems.Count > 0)
                {
                    var sceneTransform = m_scene.GetSceneTransformFor(customData.Owner.Id);
                    customData.WorldMatrix = sceneTransform.WorldMatrix;

                    m_gizmoService.UpdateSelectedObject(customData);
                }
            }
            else if (eventArgs.PropertyName == nameof(WorldOutlineData.Name))
            {
                var customData = sender as WorldOutlineData;
                m_scene.NameForEntity(customData.Owner.Id, customData.Name);

                IsModified = true;
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
            UpdateProperties();
        }

        protected override void OnLoad(string filename)
        {
            var settings = new GameContentBuilderSettings();
            settings.RootDirectory = Path.GetDirectoryName(filename);//HACK: TODO

            var gameContentBuilder = new GameContentBuilder(settings);

            settings = gameContentBuilder.Settings;

            var fileSystemStorage = new FileSystemStorage(settings.OutputDirectory);
            var assetNamePath = gameContentBuilder.GetRelativePath(filename);

            gameContentBuilder.RequestBuild(filename, assetNamePath, "ModelImporter", "ModelProcessor");
            gameContentBuilder.RunTheBuild();
            gameContentBuilder.Dispose();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            var contentManager = new ContentManager(graphicsDevice, fileSystemStorage);

            var model = contentManager.Load<Model>(assetNamePath);

            Scene = new Scene();
            m_scene.LoadFromModel(model);

            UpdateWorldOutline();
            UpdateProperties();
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
