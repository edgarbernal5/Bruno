
using BrunoWindows;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserTreeItem : ContentBrowserItem
    {
        public bool IsLoaded
        {
            get => m_isLoaded;
            set
            {
                m_isLoaded = value;
            }
        }
        private bool m_isLoaded;

        public bool IsExpanded
        {
            get => m_isExpanded;
            set
            {
                if (Set(ref m_isExpanded, value))
                {
                    m_populate.ExpandFolder(this);
                }
            }
        }
        private bool m_isExpanded;

        public bool IsSelected
        {
            get => m_isSelected;
            set
            {
                Set(ref m_isSelected, value);
            }
        }
        private bool m_isSelected;

        public ContentBrowserTreeItem Parent
        {
            get => m_parent;
            set
            {
                m_parent = value;
                NotifyOfPropertyChange();
            }
        }
        private ContentBrowserTreeItem m_parent;

        public ContentBrowserTreeItemCollection Children
        {
            get => m_children;
            set
            {
                if (m_children == value)
                    return;

                if (m_children != null)
                    m_children.Parent = null;

                m_children = value;

                if (m_children != null)
                    m_children.Parent = this;

                NotifyOfPropertyChange();
            }
        }
        private ContentBrowserTreeItemCollection m_children;
        private readonly ContentBrowserPopulateService m_populate;

        public ContentBrowserTreeItem(ContentBrowserPopulateService populate)
        {
            m_populate = populate;
        }
    }

    public class ContentBrowserTreeItemCollection : ObservableChildCollection<ContentBrowserTreeItem, ContentBrowserTreeItem>
    {
        public new ContentBrowserTreeItem Parent
    {
        get { return base.Parent; }
        protected internal set { base.Parent = value; }
    }

    public ContentBrowserTreeItemCollection()
        : base(null)
    {
    }

    protected override ContentBrowserTreeItem GetParent(ContentBrowserTreeItem child)
    {
        return child.Parent;
    }

    protected override void SetParent(ContentBrowserTreeItem child, ContentBrowserTreeItem parent)
    {
        child.Parent = parent;
    }
}
}
