using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bruno.Linq
{
    /// <summary>
    /// Provides new extension methods for traversing trees using LINQ.
    /// </summary>
    public static class TreeHelper
    {
        /// <summary>
        /// Gets the given node and its ancestors.
        /// </summary>
        /// <typeparam name="T">The type of node that is enumerated.</typeparam>
        /// <param name="node">
        /// The reference node where to start the search. (The reference node will be the first node in 
        /// the resulting enumeration.)
        /// </param>
        /// <param name="getParent">
        /// <para>
        /// A method that retrieves the parent object for a node of type <typeparamref name="T"/>.
        /// </para>
        /// <para>
        /// The method should return <see langword="null"/> to indicate that a node does not have a
        /// parent. <see cref="GetSelfAndAncestors{T}"/> guarantees that <paramref name="getParent"/> is 
        /// never called with <see langword="null"/> as parameter.
        /// </para>
        /// </param>
        /// <returns>
        /// The <paramref name="node"/> and its ancestors (along the path from the node to the root).
        /// </returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="node"/> is <see langword="null"/>.
        /// </exception>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="getParent"/> is <see langword="null"/>.
        /// </exception>
        public static IEnumerable<T> GetSelfAndAncestors<T>(T node, Func<T, T> getParent) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getParent == null)
                throw new ArgumentNullException("getParent");

            return GetSelfAndAncestorsImpl(node, getParent);
        }

        public static T GetRoot<T>(T node, Func<T, T> getParent) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getParent == null)
                throw new ArgumentNullException("getParent");

            return GetSelfAndAncestorsImpl(node, getParent).Last();
        }

        private static IEnumerable<T> GetSelfAndAncestorsImpl<T>(T node, Func<T, T> getParent) where T : class
        {
            // Start with current node.
            yield return node;

            // Same as GetAncestorsImpl().
            T parent = getParent(node);
            while (parent != null)
            {
                yield return parent;
                parent = getParent(parent);
            }
        }

        public static IEnumerable<T> GetDescendants<T>(T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getChildren == null)
                throw new ArgumentNullException("getChildren");

            return GetDescendantsImpl(node, getChildren, depthFirst);
        }

        private static IEnumerable<T> GetDescendantsImpl<T>(T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
            if (depthFirst)
            {
                Stack<T> stack = new Stack<T>();
                stack.Push(node);
                while (stack.Count > 0)
                {
                    T descendant = stack.Pop();
                    if (descendant != node)
                    {
                        yield return descendant;
                    }

                    foreach (T child in getChildren(descendant).Reverse())
                    {
                        if (child != null)
                            stack.Push(child);
                    }
                }
            }
            else
            {
                Queue<T> queue = new Queue<T>();
                queue.Enqueue(node);
                while (queue.Count > 0)
                {
                    T descendant = queue.Dequeue();
                    if (descendant != node)
                        yield return descendant;

                    foreach (T child in getChildren(descendant))
                    {
                        if (child != null)
                            queue.Enqueue(child);
                    }
                }
            }
        }

        public static IEnumerable<T> GetAncestors<T>(T node, Func<T, T> getParent) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getParent == null)
                throw new ArgumentNullException("getParent");

            return GetAncestorsImpl(node, getParent);
        }


        private static IEnumerable<T> GetAncestorsImpl<T>(T node, Func<T, T> getParent) where T : class
        {
            T parent = getParent(node);
            while (parent != null)
            {
                yield return parent;
                parent = getParent(parent);
            }
        }

        /// <overloads>
        /// <summary>
        /// Gets the subtree (the given node and all of its descendants).
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Gets the subtree (the given node and all of its descendants) using a depth-first search.
        /// </summary>
        /// <typeparam name="T">The type of node that is enumerated.</typeparam>
        /// <param name="node">
        /// The reference node where to start the search. (The reference node will be the first element
        /// in the enumeration.)
        /// </param>
        /// <param name="getChildren">
        /// <para>
        /// A method that retrieves the children of an object of type <typeparamref name="T"/>. 
        /// </para>
        /// <para>
        /// <see cref="GetSubtree{T}(T,Func{T,IEnumerable{T}})"/> guarantees that 
        /// <paramref name="getChildren"/> is never called with <see langword="null"/> as parameter. The
        /// enumeration returned by <paramref name="getChildren"/> may contain <see langword="null"/>.
        /// </para>
        /// </param>
        /// <returns>The subtree of <paramref name="node"/>.</returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="node"/> is <see langword="null"/>.
        /// </exception>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="getChildren"/> is <see langword="null"/>.
        /// </exception>
        /// <remarks>
        /// This method can be used to traverse a tree in depth-first order (preorder).
        /// </remarks>
        public static IEnumerable<T> GetSubtree<T>(T node, Func<T, IEnumerable<T>> getChildren) where T : class
        {
            return GetSubtree(node, getChildren, true);
        }


        /// <summary>
        /// Gets the subtree (the given node and all of its descendants) using a depth-first search or a
        /// breadth-first search.
        /// </summary>
        /// <typeparam name="T">The type of node that is enumerated.</typeparam>
        /// <param name="node">
        /// The reference node where to start the search. (The reference node will be the first element
        /// in the enumeration.)
        /// </param>
        /// <param name="getChildren">
        /// <para>
        /// A method that retrieves the children of an object of type <typeparamref name="T"/>. 
        /// </para>
        /// <para>
        /// <see cref="GetSubtree{T}(T,Func{T,IEnumerable{T}})"/> guarantees that 
        /// <paramref name="getChildren"/> is never called with <see langword="null"/> as parameter. The
        /// enumeration returned by <paramref name="getChildren"/> may contain <see langword="null"/>.
        /// </para>
        /// </param>
        /// <param name="depthFirst">
        /// If set to <see langword="true"/> then a depth-first search for descendants will be made; 
        /// otherwise a breadth-first search will be made.
        /// </param>
        /// <returns>The subtree of <paramref name="node"/>.</returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="node"/> is <see langword="null"/>.
        /// </exception>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="getChildren"/> is <see langword="null"/>.
        /// </exception>
        /// <remarks>
        /// This method can be used to traverse a tree in either depth-first order (preorder) or in 
        /// breadth-first order (also known as level-order).
        /// </remarks>
        public static IEnumerable<T> GetSubtree<T>(T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getChildren == null)
                throw new ArgumentNullException("getChildren");

            return GetSubtreeImpl(node, getChildren, depthFirst);
        }


        private static IEnumerable<T> GetSubtreeImpl<T>(T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
            if (depthFirst)
            {
                Stack<T> stack = new Stack<T>();
                stack.Push(node);
                while (stack.Count > 0)
                {
                    T descendant = stack.Pop();
                    yield return descendant;

                    foreach (T child in getChildren(descendant).Reverse())
                    {
                        if (child != null)
                        {
                            stack.Push(child);
                        }
                    }
                }
            }
            else
            {
                Queue<T> queue = new Queue<T>();
                queue.Enqueue(node);
                while (queue.Count > 0)
                {
                    T descendant = queue.Dequeue();
                    yield return descendant;

                    foreach (T child in getChildren(descendant))
                        if (child != null)
                            queue.Enqueue(child);
                }
            }
        }


        /// <summary>
        /// Gets the leaves of a given tree.
        /// </summary>
        /// <typeparam name="T">The type of node that is enumerated.</typeparam>
        /// <param name="node">The reference node where to start the search.</param>
        /// <param name="getChildren">
        /// <para>
        /// A method that retrieves the children of an object of type <typeparamref name="T"/>.
        /// </para>
        /// <para>
        /// <see cref="GetLeaves{T}(T,Func{T,IEnumerable{T}})"/> guarantees that 
        /// <paramref name="getChildren"/> is never called with <see langword="null"/> as parameter. The
        /// enumeration returned by <paramref name="getChildren"/> may contain <see langword="null"/>.
        /// </para>
        /// </param>
        /// <returns>The leaves of <paramref name="node"/>.</returns>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="node"/> is <see langword="null"/>.
        /// </exception>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="getChildren"/> is <see langword="null"/>.
        /// </exception>
        public static IEnumerable<T> GetLeaves<T>(T node, Func<T, IEnumerable<T>> getChildren) where T : class
        {
            if (node == null)
                throw new ArgumentNullException("node");
            if (getChildren == null)
                throw new ArgumentNullException("getChildren");

            return GetLeavesImpl(node, getChildren);
        }

        private static IEnumerable<T> GetLeavesImpl<T>(T node, Func<T, IEnumerable<T>> getChildren) where T : class
        {
            Stack<T> stack = new Stack<T>();
            stack.Push(node);
            while (stack.Count > 0)
            {
                T descendant = stack.Pop();

                bool hasChildren = false;
                var children = getChildren(descendant);
                foreach (T child in children.Reverse())
                {
                    hasChildren = true;
                    if (child != null)
                        stack.Push(child);
                }

                if (!hasChildren)
                {
                    // Leaf node.
                    yield return descendant;
                }
            }
        }
    }
}
