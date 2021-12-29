using System;

namespace Bruno.Collections
{
    public class TreeNode<T> where T : class, INamedItem
    {
        public T Content { get; set; }

        public TreeNode<T> Parent { get; internal set; }

        public TreeNodeCollection<T> Children
        {
            get => _children;
            set
            {
                if (_children == value)
                    return;

                _children = value;
            }
        }
        private TreeNodeCollection<T> _children;

        public MergePoint MergePoint { get => _mergePoint; }
        private MergePoint _mergePoint;

        public TreeNode(T content)
        {
            Content = content;
            _mergePoint = MergePoint.DefaultMergePoint;

            _children = new TreeNodeCollection<T>();
        }

        public TreeNode(T content, params TreeNode<T>[] children)
          : this(content)
        {
            if (children != null)
            {
                Children.AddRange(children);
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
                    throw new InvalidOperationException(".");

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
