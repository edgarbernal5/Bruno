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

            int indexOfExistingNode = FindTargetNode(targetNodes, node.Content.Name);
            bool nodeExists = (indexOfExistingNode >= 0);

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
                    break;
                case MergeOperation.InsertAfter:
                    break;
                case MergeOperation.InsertBefore:
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

        public MergeOperation MergeOperation { get => _operation; }
        private MergeOperation _operation;

        public string Target { get => _target; }
        private string _target;

        public MergePoint(MergeOperation operation)
        {
            _operation = operation;
            _target = null;
        }

        public MergePoint(MergeOperation operation, string target)
        {
            _operation = operation;
            _target = target;
        }
    }
}
