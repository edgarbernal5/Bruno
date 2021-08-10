
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Gizmos;
using BrunoWindows;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineData : PropertyChangedBase, ITransformable, ISelectableObject
    {
        public string Name
        {
            get; set;
        }

        [Browsable(false)]
        public WorldOutlineItem Owner { get; internal set; }

        public Vector3 LocalPosition
        {
            get
            {
                return m_localPosition;
            }
            set
            {
                m_localPosition = value;
                NotifyOfPropertyChange();
            }
        }
        private Vector3 m_localPosition;

        public Vector3 LocalRotation
        {
            get
            {
                return m_localRotation;
            }
            set
            {
                m_localRotation = value;
                NotifyOfPropertyChange();
            }
        }
        private Vector3 m_localRotation;

        public Vector3 LocalScale
        {
            get
            {
                return m_localScale;
            }
            set
            {
                m_localScale = value;
                NotifyOfPropertyChange();
            }
        }
        private Vector3 m_localScale;

        public Matrix WorldMatrix { get => m_worldMatrix; set => m_worldMatrix = value; }
        private Matrix m_worldMatrix;

        public Matrix ParentWorldMatrix { get => m_parentWorldMatrix; set => m_parentWorldMatrix = value; }
        private Matrix m_parentWorldMatrix;

        public BoundingBox BoundingBox
        {
            get
            {
                Vector3 scale, translation;
                Quaternion rotation;
                m_worldMatrix.Decompose(out scale, out rotation, out translation);

                Vector3 center = translation;// + m_center * scale;
                return new BoundingBox(center, m_extents * scale);
            }
        }

        public bool IsSelectable => m_isSelectable;

        private Vector3 m_center;
        private Vector3 m_extents;

        private bool m_isSelectable = false;

        public void SetObjectExtents(Vector3 center, Vector3 extents)
        {
            m_center = center;
            m_extents = extents;
            m_isSelectable = true;
        }
    }
}
