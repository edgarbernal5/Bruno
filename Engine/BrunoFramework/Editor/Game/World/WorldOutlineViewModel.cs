
using System.Linq;
using System.Windows.Input;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineViewModel : EditorDockTabViewModel
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

        public IWorldOutlineView View
        {
            get => m_view;
            set
            {
                m_view = value;
            }
        }
        private IWorldOutlineView m_view;

        public ICommand FindObjectCommand
        {
            get
            {
                if (m_findObjectCommand == null)
                {
                    m_findObjectCommand = new DelegateCommand(() =>
                    {
                        m_worldOutlineService.Search(m_view.SearchText);
                    });
                }
                return m_findObjectCommand;
            }
        }
        private ICommand m_findObjectCommand;


        public ICommand DeleteObjectsCommand
        {
            get
            {
                if (m_deleteObjectsCommand == null)
                {
                    m_deleteObjectsCommand = new DelegateCommand(() =>
                    {
                        if (Outline.SelectedItems.Count == 0) return;

                        var selectedEntities = Outline.SelectedItems.Select(x => x.Id).ToList();
                        m_worldOutlineService.RemoveEntities(selectedEntities);
                    });
                }
                return m_deleteObjectsCommand;
            }
        }
        private ICommand m_deleteObjectsCommand;
        public ICommand AddObjectCommand
        {
            get
            {
                if (m_addObjectCommand == null)
                {
                    m_addObjectCommand = new DelegateCommand(() =>
                    {
                        m_worldOutlineService.AddEmptyObject();
                    });
                }
                return m_addObjectCommand;
            }
        }
        private ICommand m_addObjectCommand;

        private readonly IWorldOutlineService m_worldOutlineService;

        internal static WorldOutlineViewModel DesignInstance
        {
            get
            {
                var vm = new WorldOutlineViewModel(null)
                {
                    Outline = new WorldOutline(null)
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
            m_worldOutlineService = editor.Services.GetInstance<IWorldOutlineService>();
        }
    }
}
