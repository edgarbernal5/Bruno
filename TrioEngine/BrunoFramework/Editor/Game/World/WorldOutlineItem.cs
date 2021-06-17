
using BrunoWindows;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineItem : PropertyChangedBase
    {
        public string Text
        {
            get => m_text;
            set
            {
                m_text = value;
                NotifyOfPropertyChange();
            }
        }
        private string m_text;

        public long Id
        {
            get => m_id;
            set
            {
                m_id = value;
                NotifyOfPropertyChange();
            }
        }
        private long m_id;

        public long ParentId
        {
            get => m_parentId;
            set
            {
                m_parentId = value;
                NotifyOfPropertyChange();
            }
        }
        private long m_parentId;

        public bool IsSelected
        {
            get => m_isSelected;
            set
            {
                m_isSelected = value;
                NotifyOfPropertyChange();
            }
        }
        private bool m_isSelected;

        public bool IsExpanded
        {
            get => m_isExpandedd;
            set
            {
                m_isExpandedd = value;
                NotifyOfPropertyChange();
            }
        }
        private bool m_isExpandedd;

        public WorldOutlineItem Parent
        {
            get => m_parent;
            set
            {
                m_parent = value;
                NotifyOfPropertyChange();
            }
        }
        private WorldOutlineItem m_parent;

        public WorldOutlineItemCollection Children
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
        private WorldOutlineItemCollection m_children;

    }
}
