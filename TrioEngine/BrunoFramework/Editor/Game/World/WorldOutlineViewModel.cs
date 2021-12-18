
namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineViewModel : EditorDockableTabViewModel
    {
        internal const string DockIdString = "WorldOutline";

        public WorldOutline Outline
        {
            get => m_outline;
            set
            {
                m_outline = value;
                NotifyOfPropertyChange();
            }
        }
        private WorldOutline m_outline;

        private readonly IEditorService m_editor;

        internal static WorldOutlineViewModel DesignInstance
        {
            get
            {
                var vm = new WorldOutlineViewModel(null)
                {
                    Outline = new WorldOutline
                    {
                        RootItems =
                        {
                            new WorldOutlineItem
                            {
                                Text = "Root",
                                Children = new WorldOutlineItemCollection()
                                {
                                    new WorldOutlineItem
                                    {
                                        Text = "Item 1",
                                    }
                                }
                            }
                        }
                    }
                };
                return vm;
            }
        }

        public WorldOutlineViewModel(IEditorService editor)
        {
            DisplayName = "World outline";
            DockId = DockIdString;
            m_editor = editor;

        }

    }
}
