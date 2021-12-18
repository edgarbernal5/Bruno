
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

namespace BrunoFramework.Editor.Game
{
    public class SceneDocumentViewModel : DocumentViewModel
    {
        internal const string DockIdString = "Scene";

        public new SceneDocument ProjectFile
        {
            get
            {
                return m_sceneProjectFile;
            }
        }
        private SceneDocument m_sceneProjectFile;

        public Camera Camera
        {
            get => m_camera; 
            set
            {
                m_camera = value;
                NotifyOfPropertyChange();
            }
        }
        private Camera m_camera;

        //public GizmoService GizmoService
        //{
        //    get => m_gizmoService;
        //    set
        //    {
        //        m_gizmoService = value;
        //        NotifyOfPropertyChange();
        //    }
        //}
        //private GizmoService m_gizmoService;

        public GizmoService GizmoService
        {
            get => m_sceneProjectFile.GizmoService;
            set
            {
                m_sceneProjectFile.GizmoService = value;
                //NotifyOfPropertyChange();
            }
        }

        public GizmoType GizmoMode
        {
            get
            {
                return m_gizmoType;
            }
            set
            {
                //m_sceneProjectFile.GizmoService.CurrentGizmo = value;
                if (Set(ref m_gizmoType, value))
                {
                    m_sceneProjectFile.GizmoService.CurrentGizmo = m_gizmoType;
                }
            }
        }
        private GizmoType m_gizmoType = GizmoType.Translation;

        public IList<GameGraphicsScreen> GameGraphicsScreens
        {
            get
            {
                return m_gameGraphicsScreens;
            }
        }
        private IList<GameGraphicsScreen> m_gameGraphicsScreens;

        public Scene Scene
        {
            get
            {
                return m_sceneProjectFile.Scene;
            }
        }

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


        public SceneDocumentViewModel(SceneDocument sceneProjectFile)
            : base(sceneProjectFile)
        {
            DockId = DockIdString;
            m_editor = sceneProjectFile.Editor;
            m_gameGraphicsScreens = new List<GameGraphicsScreen>();

            m_sceneProjectFile = sceneProjectFile;
        }

        private void InitializeGameScreens()
        {
            if (m_gameGraphicsScreens.Count == 0)
            {
                m_editorGameGraphicsScreen = new EditorGameGraphicsScreen();
                m_gameGraphicsScreens.Add(m_editorGameGraphicsScreen);
            }
            
            var primitivesService = m_editor.Services.GetInstance<IEditorPrimitivesService>();
            m_editorGameGraphicsScreen.GridMesh = primitivesService.GridMesh;

            m_editorGameGraphicsScreen.GizmoService = GizmoService;

            m_editorGameGraphicsScreen.Scene = m_sceneProjectFile.Scene;
            m_editorGameGraphicsScreen.Camera = Camera;
            m_editorGameGraphicsScreen.RenderPath = new RenderPathForward();
        }

        protected override void OnActivate()
        {
            BrunoApi.Net.Game.Scene.ActiveScene = m_sceneProjectFile.Scene;

            InitializeCamera();

            //var graphicsDevice = m_editor.Services.GetInstance<IGraphicsService>().GraphicsDevice;
            //GizmoService = new GizmoService(graphicsDevice);

            InitializeGameScreens();

            base.OnActivate();
        }

        private void InitializeCamera()
        {
            Camera = new Camera(new Vector3(2.0f, 4.5f, 4.5f), Vector3.Zero, Vector3.Up);
            //Camera = new Camera(new Vector3(40.0f, 60.5f, 60.5f), Vector3.Zero, Vector3.Up);
            Camera.FieldOfView = MathHelper.ToRadians(60.0f);
            Camera.NearPlane = 0.5f;
            Camera.FarPlane = 1000.0f;

            GizmoService.Camera = Camera;
        }
    }
}
