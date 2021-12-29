
using Bruno.Collections;

namespace BrunoFramework.Editor.Units
{
    public class MenuUnit : EditorUnit
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;

        public MenuUnit()
        {
            Priority = int.MaxValue;
        }

        protected override void OnInitialize()
        {

        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("Exit", new DelegateCommand(() => Editor.Exit()))
                {
                    Text = "Exit",
                }
            );
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("FileGroup", "File"),
                    new TreeNode<ICommandItem>(CommandItems["Exit"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnUninitialize()
        {

        }

        protected override void OnShutdown()
        {
            RemoveMenus();
            RemoveCommands();
        }

        private void RemoveMenus()
        {
            Editor.MenuNodes.Remove(_menuNodes);
            _menuNodes.Clear();
        }

        private void RemoveCommands()
        {
            CommandItems.Clear();
        }

        public override EditorDockableTabViewModel GetDockTabViewModel(string dockId)
        {
            return null;
        }
    }
}
