using System;

namespace Bruno.Collections
{
    public class TreeNode<T> where T : class, INamedItem
    {
        public T Content { get; set; }

        public TreeNode<T> Parent { get; internal set; }

        public TreeNodeCollection<T> Children
        {
            get => m_children;
            set
            {
                if (m_children == value)
                    return;

                m_children = value;
            }
        }
        private TreeNodeCollection<T> m_children;

        public MergePoint MergePoint { get => m_mergePoint; }
        private MergePoint m_mergePoint;


        public TreeNode(T content)
        {
            Content = content;
            m_mergePoint = MergePoint.DefaultMergePoint;

            m_children = new TreeNodeCollection<T>();
        }

        public TreeNode(T content, params TreeNode<T>[] children)
          : this(content)
        {
            if (children != null)
            {
                Children.AddRange(children);
            }
        }

        public TreeNode(T content, MergePoint mergePoint)
        {
            Content = content;
            m_mergePoint = mergePoint;

            m_children = new TreeNodeCollection<T>();
        }

        public TreeNode(T content, MergePoint mergePoint, params TreeNode<T>[] children)
          : this(content, mergePoint)
        {
            if (children != null)
            {
                m_children.AddRange(children);
            }
        }
    }

    public class TreeNodeCollection<T> : ChildCollection<TreeNode<T>, TreeNode<T>> where T : class, INamedItem
    {
        public TreeNodeCollection()
            : base(null)
        {
        }

        internal new TreeNode<T> Parent
        {
            get { return base.Parent; }
            set
            {
                if (base.Parent == value)
                    return;

                if (base.Parent != null)
                    throw new InvalidOperationException("Item has already parent");

                base.Parent = value;
            }
        }

        protected override TreeNode<T> GetParent(TreeNode<T> child)
        {
            return child.Parent;
        }

        protected override void SetParent(TreeNode<T> child, TreeNode<T> parent)
        {
            child.Parent = parent;
        }
    }
}
