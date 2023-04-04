
using System;
using System.Collections.Generic;
using System.Linq;

namespace Bruno.Linq
{
    public static class TreeHelper
    {
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

        public static IEnumerable<T> GetDescendants<T>(this T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
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

        public static IEnumerable<T> GetAncestors<T>(this T node, Func<T, T> getParent) where T : class
        {
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

        public static IEnumerable<T> GetSubtree<T>(T node, Func<T, IEnumerable<T>> getChildren) where T : class
        {
            return GetSubtree(node, getChildren, true);
        }

        public static IEnumerable<T> GetSubtree<T>(T node, Func<T, IEnumerable<T>> getChildren, bool depthFirst) where T : class
        {
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

        public static IEnumerable<T> GetLeaves<T>(T node, Func<T, IEnumerable<T>> getChildren) where T : class
        {
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
