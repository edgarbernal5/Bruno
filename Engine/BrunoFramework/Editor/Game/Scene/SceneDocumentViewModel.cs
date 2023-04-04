
using BrunoFramework.Editor.Game.Gizmos;
using BrunoFramework.Editor.Graphics;
using BrunoFramework.Editor.Units;
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System.Collections.Generic;
using BrunoApi.Net.Game;
using BrunoApi.Net.Maths;
using BrunoApi.Net.Renderer;
using System.Threading.Tasks;
using System.Threading;
using BrunoApi.Net.Graphics;
using System.Windows.Input;
using System.ComponentModel;
using System.Collections.Specialized;
using System.Linq;
using BrunoApi.Net.Graphics.Core;

namespace BrunoFramework.Editor.Game
{
    public class SceneDocumentViewModel : DocumentViewModel
    {
        internal const string DockIdString = "Scene";

        public new SceneDocument Document => m_sceneDocument;
        private SceneDocument m_sceneDocument;

        public Camera Camera
        {
            get => m_sceneDocument.Camera;
            set
            {
                m_sceneDocument.Camera = value;
                NotifyOfPropertyChange();
            }
        }

        public IGizmoService GizmoService
        {
            get => m_sceneDocument.GizmoService;
            set
            {
                m_sceneDocument.GizmoService = value;
                NotifyOfPropertyChange();
            }
        }

        public GizmoType GizmoMode
        {
            get => m_gizmoType;
            set
            {
                if (Set(ref m_gizmoType, value))
                {
                    m_sceneDocument.GizmoService.CurrentGizmo = m_gizmoType;
                    m_transformSpaceCommand.RaiseCanExecuteChanged();
                }
            }
        }
        private GizmoType m_gizmoType = GizmoType.Translation;

        public IList<GameGraphicsScreen> GameGraphicsScreens => m_gameGraphicsScreens;
        private IList<GameGraphicsScreen> m_gameGraphicsScreens;

        public Scene Scene => m_sceneDocument.Scene;

        public TransformSpace TransformSpace
        {
            get => GizmoService.TransformSpace;
            set
            {
                GizmoService.TransformSpace = value;
                NotifyOfPropertyChange();
            }
        }

        public ICommand TransformSpaceCommand
        {
            get
            {
                if (m_transformSpaceCommand == null)
                {
                    m_transformSpaceCommand = new DelegateCommand(() =>
                      {
                          TransformSpace = TransformSpace == TransformSpace.Local ?
                              TransformSpace.World :
                              TransformSpace.Local;

                      }, () => { return GizmoService.CurrentGizmo != GizmoType.Scale; });
                }
                return m_transformSpaceCommand;
            }
        }
        private DelegateCommand m_transformSpaceCommand;

        public ICommand ChangeCameraViewCommand
        {
            get
            {
                if (m_changeCameraViewCommand == null)
                {
                    m_changeCameraViewCommand = new DelegateCommand<Axis>(ChangeCameraView);
                }
                return m_changeCameraViewCommand;
            }
        }
        private ICommand m_changeCameraViewCommand;

        public bool ShowGrid
        {
            get => m_showGrid;
            set
            {
                m_showGrid = value;
                m_editorGameGraphicsScreen.ShowGrid = m_showGrid;
                NotifyOfPropertyChange();
            }
        }
        private bool m_showGrid = true;

        public ICommand ShowSnapSettingsCommand
        {
            get
            {
                if (m_showSnapSettingsCommand == null)
                {
                    m_showSnapSettingsCommand = new AsyncCommand(() =>
                        m_sceneService.ShowSnapSettingsAsync(GizmoService.SnappingConfig)
                    );
                }
                return m_showSnapSettingsCommand;
            }
        }
        private ICommand m_showSnapSettingsCommand;

        //TODO
        /*
         *  public static readonly DependencyProperty GraphicsScreensProperty = DependencyProperty.Register(
            "GraphicsScreens",
            typeof(IList<GraphicsScreen>),
            typeof(GamePresentationTarget),
            new PropertyMetadata(null, OnGraphicsScreensChanged));
         */

        private EditorGameGraphicsScreen m_editorGameGraphicsScreen;
        private readonly IEditorService m_editor;
        private readonly ISceneService m_sceneService;

        public SceneDocumentViewModel(SceneDocument sceneDocument)
            : base(sceneDocument)
        {
            DockId = DockIdString;
            m_editor = sceneDocument.Editor;
            m_gameGraphicsScreens = new List<GameGraphicsScreen>();

            m_sceneDocument = sceneDocument;
            m_sceneService = m_editor.Services.GetInstance<ISceneService>();
        }

        private void InitializeGameScreens()
        {
            if (m_editorGameGraphicsScreen == null)
            {
                var editorScreen = new EditorGameGraphicsScreen();

                var primitivesService = m_editor.Services.GetInstance<IEditorPrimitivesService>();
                editorScreen.GridMesh = primitivesService.GridMesh;

                editorScreen.GizmoService = GizmoService;
                editorScreen.ShowGrid = m_showGrid;

                editorScreen.Scene = m_sceneDocument.Scene;
                editorScreen.Camera = Camera;
                editorScreen.RenderPath = new RenderPathForward();

                m_editorGameGraphicsScreen = editorScreen;
                m_gameGraphicsScreens.Add(m_editorGameGraphicsScreen);

                InitializeDebugRenderer();
            }
        }

        private void InitializeDebugRenderer()
        {
            var debugRenderer = new DebugRenderer();

            var primitivesService = m_editor.Services.GetInstance<IEditorPrimitivesService>();
            debugRenderer.Effect = primitivesService.LineEffect;
            m_editorGameGraphicsScreen.DebugRenderer = debugRenderer;

            var gameUnitService = m_editor.Services.GetInstance<IGameUnitService>();
            gameUnitService.OnNewFrame += OnGameUnitNewFrame;
        }

        private void OnGameUnitNewFrame()
        {
            var debugRenderer = m_editorGameGraphicsScreen.DebugRenderer;
            debugRenderer.Clear();

            if (Document.WorldOutline.SelectedItems.Count == 0) return;

            var selectableObjects = Document.WorldOutline.SelectedItems.Select(item => item.CustomData)
                .Cast<WorldOutlineData>()
                .Where(item => item != null && item.IsSelectable);

            foreach (var item in selectableObjects)
            {
                var box = item.BoundingBox;

                debugRenderer.AddBoundingBox(box, ColorRGBA8.Orange);
            }
        }

        protected override Task OnInitializeAsync(CancellationToken cancellationToken)
        {
            InitializeCamera();
            InitializeGameScreens();

            Document.PropertyChanged += OnDocumentPropertyChanged;

            return base.OnInitializeAsync(cancellationToken);
        }

        protected override Task OnDeactivateAsync(bool close, CancellationToken cancellationToken)
        {
            if (close)
            {
                var gameUnitService = m_editor.Services.GetInstance<IGameUnitService>();
                gameUnitService.OnNewFrame -= OnGameUnitNewFrame;

                Document.PropertyChanged -= OnDocumentPropertyChanged;
            }
            return base.OnDeactivateAsync(close, cancellationToken);
        }

        private void OnDocumentPropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(Document.Scene))
            {
                m_editorGameGraphicsScreen.Scene = m_sceneDocument.Scene;
            }
        }

        private void InitializeCamera()
        {
            var camera = new Camera(new Vector3(2.0f, 4.5f, 4.5f), Vector3.Zero, Vector3.Up);
            camera.FieldOfView = MathHelper.ToRadians(60.0f);
            camera.NearPlane = 0.5f;
            camera.FarPlane = 1000.0f;

            var viewport = new Viewport();
            viewport.Width = 1;
            viewport.Height = 1;
            camera.Viewport = viewport;

            Camera = camera;
            GizmoService.Camera = camera;
        }

        private void ChangeCameraView(Axis axis)
        {
            var target = Camera.Target;
            var distance = (Camera.Position - target).Length();

            var axisToDirection = new Dictionary<Axis, Vector3>()
            {
                { Axis.PositiveX, Vector3.Left} ,
                { Axis.NegativeX, Vector3.Right },

                { Axis.PositiveY, Vector3.Down} ,
                { Axis.NegativeY, Vector3.Up },

                { Axis.PositiveZ, Vector3.Forward } ,
                { Axis.NegativeZ, Vector3.Backward}
            };
            var axisToRight = new Dictionary<Axis, Vector3>()
            {
                { Axis.PositiveX, Vector3.Forward } ,
                { Axis.NegativeX, Vector3.Backward },

                { Axis.PositiveY, Vector3.Right} ,
                { Axis.NegativeY, Vector3.Right },

                { Axis.PositiveZ, Vector3.Right} ,
                { Axis.NegativeZ, Vector3.Left }
            };

            var direction = axisToDirection[axis];
            var right = axisToRight[axis];
            var newCameraPosition = direction * distance;

            var upPerpendicular = Vector3.Cross(right, direction);

            Camera.Position = newCameraPosition + target;
            Camera.Up = upPerpendicular;
        }
    }

    public enum Axis
    {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    }
}
