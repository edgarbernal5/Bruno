
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
using BrunoApi.Net.Maths;
using System;
using BrunoFramework.Editor.Timing;
using BrunoFramework.Editor.Game.Inspectors;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game
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
        private IInspectorService m_inspectorService;

        public SceneProjectFile(IEditorService editor) :
            base(editor)
        {
            m_outlineService = Editor.Services.GetInstance<IWorldOutlineService>();
            m_inspectorService = Editor.Services.GetInstance<IInspectorService>();

            var graphicsDevice = Editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            GizmoService = new GizmoService(graphicsDevice, Editor.Services.GetInstance<GameStepTimer>());
            GizmoService.CurrentGizmo = GizmoType.Rotation;
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
            m_gizmoService.OnScaleChanged += OnGizmoScaleChanged;
            m_gizmoService.OnRotateChanged += OnGizmoRotateChanged;

            //TODO: hacer esto si y solo si es el documento activo (pestaña)
            m_outlineService.WorldOutline = m_worldOutline;
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
                var id = m_worldOutline.SelectedItems[0].Id;
                var sceneTransform = m_scene.GetSceneTransformFor(id);

                var customData = m_worldOutline.SelectedItems[0].CustomData as WorldOutlineData;

                //bool savedNotifiying = customData.IsNotifying;
                //customData.IsNotifying = false;
                customData.LocalPosition = sceneTransform.LocalPosition;
                customData.LocalRotation = Quaternion.EulerAngles(sceneTransform.LocalRotation);
                customData.LocalScale = sceneTransform.LocalScale;
                customData.WorldMatrix = sceneTransform.WorldMatrix;

                //customData.IsNotifying = savedNotifiying;

                m_gizmoService.SetTransformableSelected(customData);

                m_inspectorService.SelectedObject = new InspectableObjectBuilder()
                    .WithObjectProperties(customData, x => true)
                    .ToInspectableObject();
            }
        }

        private void OnGizmoTranslationChanged(ITransformable gizmoTransformable, Vector3 delta)
        {
            Console.WriteLine("translate delta = " + delta + " ; " + m_gizmoService.CurrentGizmoAxis.ToString());

            var customData = gizmoTransformable as WorldOutlineData;
            customData.LocalPosition += delta;
        }

        private void OnGizmoScaleChanged(ITransformable gizmoTransformable, Vector3 delta, bool isUniformScale)
        {
            var customData = gizmoTransformable as WorldOutlineData;

            delta *= 0.1f;
            if (isUniformScale)
            {
                float uniform = 1.0f + (delta.X + delta.Y + delta.Z) / 3.0f;
                Console.WriteLine("Uniform scale delta = " + uniform + " ; " + m_gizmoService.CurrentGizmoAxis.ToString());
                customData.LocalScale *= uniform;
                return;
            }
            Console.WriteLine("NU scale delta = " + delta + " ; " + m_gizmoService.CurrentGizmoAxis.ToString());
            customData.LocalScale += delta;
        }

        private void OnGizmoRotateChanged(ITransformable gizmoTransformable, Quaternion delta)
        {
            var customData = gizmoTransformable as WorldOutlineData;

            var localRot = Quaternion.CreateFromYawPitchRoll(customData.LocalRotation) * delta;
            localRot.Normalize();

            var deltaEulerAngles = Quaternion.EulerAngles(localRot);
            customData.LocalRotation = deltaEulerAngles;
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
                        var outlineItem = CreateOutlineItem(entityName.id, item.Parent, entityName.name);
                        var customData = new WorldOutlineData();
                        customData.Name = entityName.name;
                        outlineItem.CustomData = customData;
                        customData.Owner = outlineItem;

                        customData.PropertyChanged += CustomData_PropertyChanged;

                        m_worldOutline.RootItems.Add(outlineItem);
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
                                var outlineItem = CreateOutlineItem(entityName.id, item.Parent, entityName.name);
                                var customData = new WorldOutlineData();
                                customData.Name = entityName.name;
                                customData.Owner = outlineItem;
                                outlineItem.CustomData = customData;

                                customData.PropertyChanged += CustomData_PropertyChanged;

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

        private void CustomData_PropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(WorldOutlineData.LocalPosition))
            {
                var customData = sender as WorldOutlineData;

                m_scene.TransformSetLocalPosition(customData.Owner.Id, customData.LocalPosition);
                m_scene.Update();//hacer un update de solo los componentes involucrados?

                if (m_worldOutline.SelectedItems.Count > 0)
                {
                    var id = m_worldOutline.SelectedItems[0].Id;
                    var sceneTransform = m_scene.GetSceneTransformFor(id);

                    customData.WorldMatrix = sceneTransform.WorldMatrix;
                    m_gizmoService.SetTransformableSelected(customData);
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

                m_scene.TransformSetLocalRotation(customData.Owner.Id, Quaternion.CreateFromYawPitchRoll(customData.LocalRotation));
                //m_scene.TransformSetLocalRotation(customData.Owner.Id, customData.LocalQuatRotation);
                m_scene.Update();

                var sceneTransform = m_scene.GetSceneTransformFor(customData.Owner.Id);
                customData.WorldMatrix = sceneTransform.WorldMatrix;
                //m_gizmoService.SetWorld(customData.WorldMatrix);
            }
        }

        Vector3 GetRotation(Matrix rotation)
        {
            Vector3 angles = Vector3.Zero;
            if (rotation.M11 == 1.0f)
            {
                angles.Y = (float)Math.Atan2(rotation.M13, rotation.M34);
                angles.X = 0;
                angles.Z = 0;

            } else if (rotation.M11 == -1.0f)
            {
                angles.Y = (float)Math.Atan2(rotation.M13, rotation.M34);
                angles.X = 0;
                angles.Z = 0;
            } else
            {
                angles.Y = (float)Math.Atan2(-rotation.M31, rotation.M11);
                angles.X = (float)Math.Asin(rotation.M21);
                angles.Z = (float)Math.Atan2(-rotation.M23, rotation.M22);
            }
            return angles;
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
