
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System.Collections.Generic;
using TrioApi.Net.Maths;
using TrioApi.Net.Renderer;

namespace EsteroFramework.Editor.Game
{
    public class SceneViewModel : Screen
    {
        public SceneProjectFile SceneProjectFile { get; set; }

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
        private List<GameGraphicsScreen> m_gameGraphicsScreens;

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

        public SceneViewModel(IEditorService editor)
        {
            m_editor = editor;
            m_gameGraphicsScreens = new List<GameGraphicsScreen>();
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
            m_editorGameGraphicsScreen.Scene = SceneProjectFile.Scene;
            m_editorGameGraphicsScreen.Camera = Camera;
            m_editorGameGraphicsScreen.RenderPath = new RenderPathForward();
        }

        protected override void OnActivate()
        {
            SceneProjectFile = new SceneProjectFile();
            SceneProjectFile.New();

            TrioApi.Net.Game.Scene.ActiveScene = SceneProjectFile.Scene;

            InitializeCamera();
            InitializeGameScreens();

            base.OnActivate();
        }

        private void InitializeCamera()
        {
            Camera = new Camera();
            Camera.FieldOfView = 60.0f * 3.1416f / 180.0f;
            Camera.NearPlane = 0.1f;
            Camera.FarPlane = 1000.0f;

            Camera.Position = new Vector3(5.0f, 5.0f, 5.0f);
            Camera.Target = Vector3.Zero;
            Camera.Up = Vector3.Up;
        }
    }
}
