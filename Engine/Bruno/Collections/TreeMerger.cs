using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bruno.Collections
{
    public class TreeMerger<T> where T : class, INamedItem
    {
        public void Merge(TreeNodeCollection<T> targetNodes, TreeNodeCollection<T> nodesToAdd)
        {
            var nodesToAddArray = nodesToAdd.ToArray();
            foreach (var node in nodesToAddArray)
            {
                Merge(targetNodes, node);
            }
        }

        private void Merge(TreeNodeCollection<T> targetNodes, TreeNode<T> node)
        {
            var mergePoint = node.MergePoint;
            var mergeOperation = mergePoint.MergeOperation;
            var mergeTarget = mergePoint.Target;

            int indexOfExistingNode = FindTargetNode(targetNodes, node.Content.Name);
            bool nodeExists = indexOfExistingNode >= 0;

            int indexOfTarget = -1;
            bool targetFound = false;

            if (!nodeExists && mergeOperation != MergeOperation.Append && mergeOperation != MergeOperation.Prepend)
            {
                indexOfTarget = FindTargetNode(targetNodes, mergeTarget);
                targetFound = indexOfTarget >= 0;

                if (!targetFound)
                {
                    return;
                }
            }

            switch (mergeOperation)
            {
                case MergeOperation.Append:
                    if (nodeExists)
                    {
                        var targetNode = targetNodes[indexOfExistingNode];
                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(targetNode.Children, node.Children);
                        }
                    }
                    else
                    {
                        var newNode = new TreeNode<T>(node.Content);
                        targetNodes.Add(newNode);

                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(newNode.Children, node.Children);
                        }
                    }
                    break;
                case MergeOperation.Prepend:
                    if (nodeExists)
                    {
                        var targetNode = targetNodes[indexOfExistingNode];
                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(targetNode.Children, node.Children);
                        }
                    }
                    else
                    {
                        var newNode = new TreeNode<T>(node.Content);
                        targetNodes.Insert(0, newNode);

                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(newNode.Children, node.Children);
                        }
                    }
                    break;

                case MergeOperation.InsertAfter:
                    if (nodeExists)
                    {
                        var targetNode = targetNodes[indexOfExistingNode];
                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(targetNode.Children, node.Children);
                        }
                    }
                    else
                    {
                        var newNode = new TreeNode<T>(node.Content);
                        targetNodes.Insert(indexOfTarget + 1, newNode);

                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(newNode.Children, node.Children);
                        }
                    }
                    break;

                case MergeOperation.InsertBefore:
                    if (nodeExists)
                    {
                        var targetNode = targetNodes[indexOfExistingNode];
                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(targetNode.Children, node.Children);
                        }
                    }
                    else
                    {
                        var newNode = new TreeNode<T>(node.Content);
                        targetNodes.Insert(indexOfTarget, newNode);

                        if (node.Children != null && node.Children.Count > 0)
                        {
                            Merge(newNode.Children, node.Children);
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        private int FindTargetNode(TreeNodeCollection<T> nodes, string target)
        {
            int index;
            var numberOfNodes = nodes.Count;
            for (index = 0; index < numberOfNodes; index++)
                if (nodes[index].Content.Name == target)
                    break;

            if (index == numberOfNodes)
            {
                return -1;
            }

            return index;
        }
    }

    public enum MergeOperation
    {
        Prepend,
        InsertAfter,
        InsertBefore,
        Append
    }

    public class MergePoint
    {
        public static readonly MergePoint Append = new MergePoint(MergeOperation.Append);
        public static readonly MergePoint Prepend = new MergePoint(MergeOperation.Prepend);
        internal static readonly MergePoint DefaultMergePoint = Append;

        public MergeOperation MergeOperation { get => m_operation; }
        private MergeOperation m_operation;

        public string Target { get => m_target; }
        private string m_target;

        public MergePoint(MergeOperation operation)
        {
            m_operation = operation;
            m_target = null;
        }

        public MergePoint(MergeOperation operation, string target)
        {
            m_operation = operation;
            m_target = target;
        }
    }
}
