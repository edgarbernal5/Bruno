
using BrunoApi.Net.IO;
using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserPopulateService
    {
        private readonly ContentBrowser m_contentBrowser;
        private readonly IStorage m_storage;

        public ContentBrowserLayout Layout { get; set; } = ContentBrowserLayout.TwoColumns;

        public ContentBrowserPopulateService(ContentBrowser contentBrowser, IStorage storage)
        {
            m_contentBrowser = contentBrowser;
            m_storage = storage;
        }

        internal void ExpandFolder(ContentBrowserTreeItem treeItem)
        {
            if (treeItem.IsLoaded) return;

            var children = treeItem.Children;
            treeItem.IsLoaded = true;
            children.RemoveAt(0);

            PopulateFolderTree(treeItem.RelativePath, treeItem.Children);
        }

        public void Execute(string relativePath = "", FilterByType filterByType = FilterByType.All, bool populateFolderTree = false, string keywordSearch = "")
        {
            m_contentBrowser.ClearFileList();
            PopulateListFile(relativePath, filterByType, keywordSearch);
            if (populateFolderTree && Layout != ContentBrowserLayout.FileTree)
            {
                m_contentBrowser.ClearFileTree();
                PopulateFolderTree();
            }
        }

        private void PopulateListFile(string relativePath, FilterByType filterByType, string keywordSearch)
        {
            if (Layout == ContentBrowserLayout.FileList)
            {
                foreach (var path in m_storage.GetDirectories(relativePath))
                {
                    if (filterByType == FilterByType.All)
                    {
                        m_contentBrowser.FileListItems.Items.Add
                        (
                            new ContentBrowserItem()
                            {
                                Text = Path.GetFileName(path),
                                Icon = GetIconFor("folder"),
                                IsFolder = true,
                                RelativePath = path, //HACK: Change this!!
                                AbsolutePath = m_storage.GetRealPath(path) //HACK: Change this!!
                            }
                        );
                    }
                }
            }
            

            foreach (var path in m_storage.GetFiles(relativePath))
            {
                if (filterByType == FilterByType.All || FilterItemByType(path, filterByType))
                {
                    m_contentBrowser.FileListItems.Items.Add
                    (
                        new ContentBrowserItem()
                        {
                            Text = Path.GetFileName(path),
                            Icon = GetIconFor("file"),
                            RelativePath = path, //HACK: Change this!!
                            AbsolutePath = m_storage.GetRealPath(path) //HACK: Change this!!
                        }
                    );
                }
            }
        }

        private void PopulateFolderTree()
        {
            var children = new ContentBrowserTreeItemCollection();
            if (m_storage.HasDirectories(""))
            {
                children.Add(new ContentBrowserTreeItem(this)
                {
                    Text = "loading...",
                    IsLoaded = false
                });

            }
            m_contentBrowser.FileTreeItems.Items.Add(
                new ContentBrowserTreeItem(this)
                {
                    Text = "Content",
                    Icon = GetIconFor("file"),
                    IsFolder = true,
                    RelativePath = "", //HACK: Change this!!
                    AbsolutePath = m_storage.GetRealPath(""), //HACK: Change this!!6
                    Children = children
                }
            );
            
        }

        private void PopulateFolderTree(string relativePath, ObservableCollection<ContentBrowserTreeItem> container)
        {
            foreach (var path in m_storage.GetDirectories(relativePath))
            {
                var children = new ContentBrowserTreeItemCollection();
                if (m_storage.HasDirectories(path))
                {
                    children.Add(new ContentBrowserTreeItem(this)
                    {
                        Text = "loading...",
                        IsLoaded = false
                    });
                }
                container.Add
                    (
                        new ContentBrowserTreeItem(this)
                        {
                            Text = Path.GetFileName(path),
                            Icon = GetIconFor("folder"),
                            IsFolder = true,
                            RelativePath = path, //HACK: Change this!!
                            AbsolutePath = m_storage.GetRealPath(path), //HACK: Change this!!

                            Children = children
                        }
                    );
            }
        }

        private bool FilterItemByType(string path, FilterByType filterByType)
        {
            string fileExtension = Path.GetExtension(path);

            switch (filterByType)
            {
                case FilterByType.Scenes:
                    return fileExtension == ".fbx" || fileExtension == ".x" || fileExtension == ".obj";
                case FilterByType.Textures:
                    return fileExtension == ".png" || fileExtension == ".tga" || fileExtension == ".bmp";
                default:
                    return false;
            }
        }

        private ImageSource GetIconFor(string filename)
        {
            var yourImage = new BitmapImage(new Uri(string.Format("Resources/Icons/{0}.jpg", filename), UriKind.Relative));
            yourImage.Freeze(); // -> to prevent error: "Must create DependencySource on same Thread as the DependencyObject"

            return yourImage;
        }
    }

    public enum ContentBrowserLayout
    {
        TwoColumns,
        FileTree,
        FileList
    }
}
