
using Bruno.Collections;
using Bruno.Linq;
using BrunoFramework.Editor.Game;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;

namespace BrunoFramework.Editor.Units
{
    public class WorldOutlineUnit : EditorUnit, IWorldOutlineService
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;

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
            Editor.Services.RegisterInstance(typeof(IWorldOutlineService), this);
            m_worldOutlineViewModel = new WorldOutlineViewModel(Editor);
        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("OpenWorldOutline", new DelegateCommand(() => OpenView()))
                {
                    Text = "World outline",
                }
            );
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("WindowsGroup", "Windows"),
                    new TreeNode<ICommandItem>(CommandItems["OpenWorldOutline"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IWorldOutlineService));
        }

        protected override void OnShutdown()
        {
            RemoveMenus();
            RemoveCommands();
            m_worldOutlineViewModel = null;
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

        public override EditorDockTabViewModel GetDockTabViewModel(string dockId)
        {
            if (dockId == WorldOutlineViewModel.DockIdString)
            {
                return m_worldOutlineViewModel;
            }

            return base.GetDockTabViewModel(dockId);
        }

        private void OpenView()
        {
            Editor.ActivateItem(m_worldOutlineViewModel);
        }

        public void Search(string keyword)
        {
            //https://docs.microsoft.com/en-us/dotnet/csharp/how-to/search-strings
            //https://stackoverflow.com/questions/28647002/find-most-accurate-match-in-strings

            if (string.IsNullOrEmpty(keyword))
            {
                var allNodesVisible = m_worldOutlineViewModel.Outline.RootItems.SelectMany(item => item.GetDescendants(y => y.Children, true));

                foreach (var node in allNodesVisible)
                {
                    node.IsVisible = true;
                }
                return;
            }
            if (m_worldOutlineViewModel.Outline.RootItems.Count == 0) return;

            var foundNodes = new List<WorldOutlineItem>();
            Search(keyword, m_worldOutlineViewModel.Outline.RootItems, foundNodes);

            var allNodes = m_worldOutlineViewModel.Outline.RootItems.Concat(m_worldOutlineViewModel.Outline.RootItems.SelectMany(item => item.GetDescendants(y => y.Children, true)));

            foreach (var item in allNodes.Where(x => !foundNodes.Contains(x)))
            {
                item.IsVisible = false;
            }

            foreach (var node in foundNodes)
            {
                foreach (var ancestor in node.GetAncestors(y => y.Parent))
                {
                    ancestor.IsVisible = true;
                }
                node.IsVisible = true;
            }
        }


        private void Search(string keyword, Collection<WorldOutlineItem> items, List<WorldOutlineItem> foundCollection)
        {
            if (items == null || items.Count == 0) return;

            foreach (var item in items)
            {
                if (item.Text.Contains(keyword))
                {
                    foundCollection.Add(item);
                }

                Search(keyword, item.Children, foundCollection);
            }
        }

        public void RemoveEntities(List<long> entities)
        {
            if (WorldOutline == null) return;

            WorldOutline.OnDeleteObjects(entities);
        }

        public void AddEmptyObject()
        {
            if (WorldOutline == null) return;

            WorldOutline.AddEmptyObject("Object");
        }
    }
}
