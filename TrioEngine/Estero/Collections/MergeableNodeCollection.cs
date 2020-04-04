using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Estero.Collections
{
    /// <summary>
    /// Manages a collection of <see cref="MergeableNode{T}"/> objects.
    /// </summary>
    /// <typeparam name="T">The type of content.</typeparam>
    public sealed class MergeableNodeCollection<T> : ChildCollection<MergeableNode<T>, MergeableNode<T>> where T : class, INamedItem
    {
        /// <overloads>
        /// <summary>
        /// Initializes a new instance of the <see cref="MergeableNodeCollection{T}"/> class.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Initializes a new instance of the <see cref="MergeableNodeCollection{T}"/> class
        /// which has no owner.
        /// </summary>
        public MergeableNodeCollection()
            : base(null)
        {
        }


        /// <summary>
        /// Gets or sets the parent which owns this child collection.
        /// </summary>
        /// <value>The parent.</value>
        internal new MergeableNode<T> Parent
        {
            get { return base.Parent; }
            set
            {
                if (base.Parent == value)
                    return;

                if (base.Parent != null)
                    throw new InvalidOperationException("Cannot assign MergeableNodeCollection<T> to MergeableNode<T>. The specified collection is already owned by another node.");

                base.Parent = value;
            }
        }


        protected override MergeableNode<T> GetParent(MergeableNode<T> child)
        {
            return child.Parent;
        }


        protected override void SetParent(MergeableNode<T> child, MergeableNode<T> parent)
        {
            child.Parent = parent;
        }
    }
}
