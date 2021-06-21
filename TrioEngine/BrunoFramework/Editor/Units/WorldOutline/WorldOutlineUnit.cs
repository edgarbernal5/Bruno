
using BrunoFramework.Editor.Game;

namespace BrunoFramework.Editor.Units
{
    public class WorldOutlineUnit : EditorUnit, IWorldOutlineService
    {
        public WorldOutline WorldOutline
        {
            get => m_worldOutlineViewModel.Outline;
            set => m_worldOutlineViewModel.Outline = value;
        }
        private WorldOutlineViewModel m_worldOutlineViewModel;

        public WorldOutlineViewModel ViewModel => m_worldOutlineViewModel;

        public WorldOutlineUnit()
        {
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IWorldOutlineService), null, this);
            m_worldOutlineViewModel = new WorldOutlineViewModel(Editor);
        }

        protected override void OnStartup()
        {

        }
        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IWorldOutlineService));
        }

        protected override void OnShutdown()
        {
            m_worldOutlineViewModel = null;
        }

    }
}
