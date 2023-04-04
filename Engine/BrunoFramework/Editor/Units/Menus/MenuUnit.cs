
using Bruno.Collections;

namespace BrunoFramework.Editor.Units
{
    public class MenuUnit : EditorUnit
    {
        private TreeNodeCollection<ICommandItem> m_menuNodes;

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
            m_menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                //File
                    new TreeNode<ICommandItem>(new CommandGroup("FileGroup", "File"),
                    new TreeNode<ICommandItem>(new CommandSeparator("CloseSeparator")),
                    new TreeNode<ICommandItem>(new CommandSeparator("SaveSeparator")),
                        new TreeNode<ICommandItem>(CommandItems["Exit"])),

                //Windows
                 new TreeNode<ICommandItem>(new CommandGroup("WindowsGroup", "Windows"))
            };

            Editor.MenuNodes.Add(m_menuNodes);
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
            Editor.MenuNodes.Remove(m_menuNodes);
            m_menuNodes.Clear();
        }

        private void RemoveCommands()
        {
            CommandItems.Clear();
        }
    }
}
