
using EsteroFramework.Editor.Game.Gizmos;
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System.Collections.Generic;
using System.ComponentModel;
using TrioApi.Net.Maths;
using TrioApi.Net.Renderer;

namespace EsteroFramework.Editor.Game
{
    public class SceneViewModel : ProjectFileViewModel
    {
        public new SceneProjectFile ProjectFile
        {
            get
            {
                return (SceneProjectFile)base.ProjectFile;
            }
        }

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

        public IList<GameGraphicsScreen> GameGraphicsScreens
        {
            get
            {
                return m_gameGraphicsScreens;
            }
        }
        private IList<GameGraphicsScreen> m_gameGraphicsScreens = new List<GameGraphicsScreen>();

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

        public SceneViewModel(SceneProjectFile sceneProjectFile)
            : base(sceneProjectFile)
        {
            m_editor = sceneProjectFile.Editor;
            //m_gameGraphicsScreens = new List<GameGraphicsScreen>();
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

            var gizmoService = m_editor.Services.GetInstance<IGizmoService>();
            m_editorGameGraphicsScreen.AxisGizmoRenderer = gizmoService.AxisGizmoRenderer;

            m_editorGameGraphicsScreen.Scene = ProjectFile.Scene;
            m_editorGameGraphicsScreen.Camera = Camera;
            m_editorGameGraphicsScreen.RenderPath = new RenderPathForward();
        }

        protected override void OnActivate()
        {
            TrioApi.Net.Game.Scene.ActiveScene = ProjectFile.Scene;

            InitializeCamera();
            InitializeGameScreens();

            base.OnActivate();
        }

        private void InitializeCamera()
        {
            Camera = new Camera(new Vector3(20.0f, 50.0f, 50.0f), Vector3.Zero, Vector3.Up);
            Camera.FieldOfView = 60.0f * 3.1416f / 180.0f;
            Camera.NearPlane = 0.1f;
            Camera.FarPlane = 1000.0f;
        }
    }
}
