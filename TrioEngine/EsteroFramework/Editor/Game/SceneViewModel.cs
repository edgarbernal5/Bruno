
using EsteroFramework.Editor.Graphics;
using EsteroFramework.Graphics.Editor;
using System.Collections.Generic;

namespace EsteroFramework.Editor.Game
{
    public class SceneViewModel : Screen
    {
        public SceneProjectFile SceneProjectFile { get; set; }

        public IList<GameGraphicsScreen> GameGraphicsScreens
        {
            get
            {
                return m_gameGraphicsScreens;
            }
        }
        private List<GameGraphicsScreen> m_gameGraphicsScreens;

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
        }

        protected override void OnActivate()
        {
            SceneProjectFile = new SceneProjectFile();
            SceneProjectFile.New();

            InitializeGameScreens();

            base.OnActivate();
        }
    }
}
