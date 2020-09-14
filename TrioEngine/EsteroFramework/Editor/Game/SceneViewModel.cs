
using EsteroFramework.Editor.Graphics;
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

        public SceneViewModel()
        {
            m_gameGraphicsScreens = new List<GameGraphicsScreen>();

            InitializeGameScreens();
        }

        private void InitializeGameScreens()
        {
            if (m_gameGraphicsScreens.Count == 0)
            {
                m_editorGameGraphicsScreen = new EditorGameGraphicsScreen();
                m_gameGraphicsScreens.Add(m_editorGameGraphicsScreen);
            }
        }

        protected override void OnActivate()
        {
            SceneProjectFile = new SceneProjectFile();
            SceneProjectFile.New();

            m_editorGameGraphicsScreen.Scene = SceneProjectFile.Scene;
            base.OnActivate();
        }
    }
}
