
using Bruno.Collections;
using BrunoApi.Net.IO;
using System;
using System.Collections.Specialized;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserUnit : EditorUnit, IContentBrowserService
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;

        public ContentBrowser ContentBrowser
        {
            get => m_contentBrowserViewModel.Content;
            set => m_contentBrowserViewModel.Content = value;
        }
        private ContentBrowserViewModel m_contentBrowserViewModel;

        public ContentBrowserViewModel ViewModel => m_contentBrowserViewModel;
        private IDocumentService m_documentService;
        private ContentBrowserPopulateService m_contentBrowserPopulate;

        public ContentBrowserUnit()
        {
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IContentBrowserService), this);
            m_contentBrowserViewModel = new ContentBrowserViewModel(Editor);
        }

        protected override void OnStartup()
        {
            m_documentService = Editor.Services.GetInstance<IDocumentService>();
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("OpenContentBrowser", new DelegateCommand(() => OpenView()))
                {
                    Text = "Content Browser",
                }
            );
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("WindowsGroup", "Windows"),
                    new TreeNode<ICommandItem>(CommandItems["OpenContentBrowser"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IContentBrowserService));
            m_documentService = null;
            m_contentBrowserPopulate = null;
        }

        protected override void OnShutdown()
        {
            DisposeContentBrowser();
            RemoveMenus();
            RemoveCommands();
            m_contentBrowserViewModel = null;
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
            if (dockId == ContentBrowserViewModel.DockIdString)
                return m_contentBrowserViewModel;

            return base.GetDockTabViewModel(dockId);
        }

        private void OpenView()
        {
            Editor.ActivateItem(m_contentBrowserViewModel);
        }

        public void Search(string keyword)
        {

        }

        public void Load(string rootDirectory)
        {
            DisposeContentBrowser();
            var contentBrowser = new ContentBrowser();

            contentBrowser.DoubleClickFileItem += OnContentBrowserDoubleClickFileItem;
            contentBrowser.DoubleClickFolderItem += OnContentBrowserDoubleClickFolderItem;
            contentBrowser.ChangedFilterByType += OnContentBrowserChangedFilterByType;
            contentBrowser.FileTreeItems.SelectedItems.CollectionChanged += OnContentBrowserTreeItemChanged;

            ContentBrowser = contentBrowser;

            PopulateContentBrowser(rootDirectory);
        }

        private void OnContentBrowserDoubleClickFileItem(ContentBrowserItem item)
        {
            var absolutePath = item.AbsolutePath;
            if (!m_documentService.CanOpen(absolutePath))
            {
                return;
            }

            m_documentService.Open(absolutePath);
        }

        private ContentBrowserTreeItem FindFolderInTree(ContentBrowserTreeItemCollection children, string path, string targetPath)
        {
            if (children == null || children.Count == 0) return null;

            foreach (var item in children)
            {
                if (item.RelativePath == targetPath)
                    return item;

                item.IsExpanded = true;
                var subTree = FindFolderInTree(item.Children, item.RelativePath, targetPath);
                if (subTree != null) return subTree;
            }

            return null;
        }

        private void OnContentBrowserDoubleClickFolderItem(string path)
        {
            ContentBrowserTreeItem newSelected = ContentBrowser.FileTreeItems.Items[0];
            if (!string.IsNullOrEmpty(path))
            {
                newSelected.IsExpanded = true;
                newSelected = FindFolderInTree(ContentBrowser.FileTreeItems.Items[0].Children, "", path);
            }

            ContentBrowser.FileTreeItems.SelectedItems.Clear();
            ContentBrowser.FileTreeItems.SelectedItems.Add(newSelected);
            m_contentBrowserPopulate.Execute(path, ContentBrowser.FilterByType);
        }

        private void OnContentBrowserChangedFilterByType(FilterByType filterByType)
        {
            if (m_contentBrowserPopulate == null) return; //HACK, TODO
            m_contentBrowserPopulate.Execute(ContentBrowser.CurrentPath, filterByType);
        }

        private void OnContentBrowserTreeItemChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (ContentBrowser.FileTreeItems.SelectedItems.Count > 0)
            {
                var itemSelected = ContentBrowser.FileTreeItems.SelectedItems[0];
                m_contentBrowserPopulate.Execute(itemSelected.RelativePath, ContentBrowser.FilterByType);
            }
        }

        private void PopulateContentBrowser(string rootDirectory)
        {
            if (string.IsNullOrEmpty(rootDirectory))
            {
                return;
            }

            m_contentBrowserPopulate = new ContentBrowserPopulateService(ContentBrowser, new FileSystemStorage(rootDirectory));
            m_contentBrowserPopulate.Layout = ContentBrowserLayout.TwoColumns;
            m_contentBrowserPopulate.Execute(populateFolderTree: true);
        }

        private void DisposeContentBrowser()
        {
            if (ContentBrowser != null)
            {
                ContentBrowser.DoubleClickFileItem -= OnContentBrowserDoubleClickFileItem;
                ContentBrowser.DoubleClickFolderItem -= OnContentBrowserDoubleClickFolderItem;
                ContentBrowser.ChangedFilterByType -= OnContentBrowserChangedFilterByType;
                ContentBrowser.FileTreeItems.SelectedItems.CollectionChanged -= OnContentBrowserTreeItemChanged;

                ContentBrowser.ClearAll();
                ContentBrowser.Dispose();
            }
        }
    }
}
