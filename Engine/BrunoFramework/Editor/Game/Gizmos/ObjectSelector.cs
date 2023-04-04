
using Bruno.Logging;
using BrunoApi.Net.Maths;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public class ObjectSelector
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        private readonly WorldOutline m_worldOutline;
        private List<IEditorObject> m_selectableObjects;

        public List<IEditorObject> SelectedObjects
        {
            get { return m_selectedObjects; }
        }
        private List<IEditorObject> m_selectedObjects;

        public ObjectSelector(WorldOutline worldOutline)
        {
            m_worldOutline = worldOutline;
            m_selectedObjects = new List<IEditorObject>();
            m_selectableObjects = new List<IEditorObject>();
        }

        private void Traverse(Collection<WorldOutlineItem> collection)
        {
            if (collection == null || collection.Count == 0)
                return;

            for (int i = 0; i < collection.Count; i++)
            {
                var item = collection[i];
                var selectableObject = item.CustomData as IEditorObject;
                if (selectableObject != null && selectableObject.IsSelectable)
                    m_selectableObjects.Add(selectableObject);

                Traverse(item.Children);
            }
        }

        public void Select(IEditorObject editorObject)
        {
            m_selectedObjects.Clear();
            m_selectedObjects.Add(editorObject);
        }

        public void Select(Ray selectionRay)
        {
            m_selectableObjects.Clear();
            Traverse(m_worldOutline.RootItems);

            var copiedSelectedItems = new List<WorldOutlineItem>(m_worldOutline.SelectedItems);

            if (copiedSelectedItems.Count == 0)
            {
                m_selectedObjects.Clear();

                for (int i = 0; i < m_selectableObjects.Count; i++)
                {
                    var selectableObject = m_selectableObjects[i];

                    var intersection = selectionRay.Intersects(selectableObject.BoundingBox);
                    if (intersection.HasValue)
                    {
                        m_selectedObjects.Add(selectableObject);
                        break;
                    }
                }

                if (m_selectedObjects.Count > 0)
                {
                    var outlineData = m_selectedObjects[0] as WorldOutlineData;
                    ExpandItem(outlineData.Owner);

                    m_worldOutline.SelectedItems.Add(outlineData.Owner);
                }
            }
            else
            {
                var selectableObject = copiedSelectedItems[0].CustomData as IEditorObject;
                m_worldOutline.SelectedItems.Clear();

                int index = Math.Max(0, m_selectableObjects.IndexOf(selectableObject));
                
                int end = index;
                int i = (index - 1 + m_selectableObjects.Count) % m_selectableObjects.Count;

                m_selectedObjects.Clear();
                while (i != end)
                {
                    var intersection = selectionRay.Intersects(m_selectableObjects[i].BoundingBox);
#if DEBUG
                    var outlineData = (m_selectableObjects[i] as WorldOutlineData);
                    Logger.Debug("object {0} = {1}", i, outlineData.Name);
#endif
                    if (intersection.HasValue)
                    {
                        m_selectedObjects.Add(m_selectableObjects[i]);
                        break;
                    }

                    i = (i - 1 + m_selectableObjects.Count) % m_selectableObjects.Count;
                }

                if (m_selectedObjects.Count > 0)
                {
                    var outlineData = m_selectedObjects[0] as WorldOutlineData;
                    ExpandItem(outlineData.Owner);
#if DEBUG
                    Logger.Debug("hit {0} = {1}", i, outlineData.Name);
#endif
                    m_worldOutline.SelectedItems.Add(outlineData.Owner);
                }

            }
        }

        private void ExpandItem(WorldOutlineItem item)
        {
            if (item.Parent == null)
                return;

            item.Parent.IsExpanded = true;
            ExpandItem(item.Parent);
        }
    }
}
