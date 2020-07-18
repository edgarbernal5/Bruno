using Estero.Collections;
using System.Collections.Generic;

namespace EsteroFramework.Editor
{
    public class MenuManager
    {
        public MenuItemViewModelCollection Menu { get; private set; }

        public TreeNodeCollection<ICommandItem> Nodes { get; }
        public List<ICommandItem> CommandItems { get; }

        public MenuManager()
        {
            Menu = new MenuItemViewModelCollection();
            Nodes = new TreeNodeCollection<ICommandItem>();
            CommandItems = new List<ICommandItem>();
        }

        internal void Update(List<TreeNodeCollection<ICommandItem>> menuNodes)
        {
            Nodes.Clear();
            MergeNodes(Nodes, menuNodes);

            CommandItems.Clear();
            CollectCommandItems(Nodes);

            Menu.Clear();
            CreateMenuItems(Nodes, Menu);
        }

        private void MergeNodes(TreeNodeCollection<ICommandItem> targetNodes, List<TreeNodeCollection<ICommandItem>> nodesToAdd)
        {
            TreeMerger<ICommandItem> treeMerger = new TreeMerger<ICommandItem>();
            foreach (var nodeCollection in nodesToAdd)
            {
                treeMerger.Merge(targetNodes, nodeCollection);
            }
        }

        private void CollectCommandItems(TreeNodeCollection<ICommandItem> nodes)
        {
            if (nodes == null || nodes.Count == 0)
                return;

            foreach (var node in nodes)
            {
                var content = node.Content;
                if (content is CommandGroup || content is CommandSeparator)
                    continue;

                CommandItems.Add(content);

                CollectCommandItems(node.Children);
            }
        }

        private void CreateMenuItems(TreeNodeCollection<ICommandItem> nodes, MenuItemViewModelCollection menuItems)
        {
            if (nodes == null || nodes.Count == 0)
                return;


            foreach (var node in nodes)
            {
                var menuItem = node.Content.CreateMenuItem();
                menuItems.Add(menuItem);

                if (node.Children != null && node.Children.Count > 0)
                {
                    menuItem.IsVisible = true;
                    menuItem.Submenu = new MenuItemViewModelCollection();
                    CreateMenuItems(node.Children, menuItem.Submenu);
                }
            }
        }
    }
}
