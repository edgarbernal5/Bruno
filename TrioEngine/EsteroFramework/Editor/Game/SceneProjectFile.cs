
using EsteroFramework.Editor.Units;
using TrioWpfFramework.Net.Game;

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

        public SceneProjectFile()
        {

        }

        protected override void OnNew()
        {
            m_scene = new Scene();
        }

        protected override void OnLoad(string filename)
        {

        }

        protected override void OnSave()
        {

        }
    }
}
