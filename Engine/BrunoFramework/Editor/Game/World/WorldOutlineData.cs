
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Gizmos;
using BrunoWindows;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineData : PropertyChangedBase, IEditorObject
    {
        public string Name
        {
            get
            {
                return Owner.Text;
            }
            set
            {
                Owner.Text = value;
                NotifyOfPropertyChange();
            }
        }

        [Browsable(false)]
        public WorldOutlineItem Owner { get; internal set; }

        [DisplayName("Position")]
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

        [DisplayName("Rotation")]
        public Quaternion LocalRotation
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
        private Quaternion m_localRotation;

        [DisplayName("Scale")]
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

        public BoundingBox BoundingBox
        {
            get
            {
                var min = Vector3.Transform(m_boxCorners[0], m_worldMatrix);
                var max = min;

                for (int i = 1; i < 8; i++)
                {
                    min = Vector3.Min(min, Vector3.Transform(m_boxCorners[i], m_worldMatrix));
                    max = Vector3.Max(max, Vector3.Transform(m_boxCorners[i], m_worldMatrix));
                }

                var extents = (max - min) * 0.5f;
                var center = (max + min) * 0.5f;

                return new BoundingBox(center, extents);
            }
        }
        private Vector3[] m_boxCorners;
        private Vector3 m_center;
        private Vector3 m_extents;

        public bool IsSelectable => m_isSelectable;
        private bool m_isSelectable = false;


        public void SetObjectExtents(Vector3 center, Vector3 extents)
        {
            m_center = center;
            m_extents = extents;
            m_isSelectable = true;
            m_boxCorners = new Vector3[8];

            Vector3 min = m_center - m_extents;
            Vector3 max = m_center + m_extents;

            m_boxCorners[0] = min;
            m_boxCorners[1] = new Vector3(min.X, max.Y, min.Z);
            m_boxCorners[2] = new Vector3(max.X, min.Y, min.Z);
            m_boxCorners[3] = new Vector3(max.X, max.Y, min.Z);

            m_boxCorners[4] = max;
            m_boxCorners[5] = new Vector3(min.X, max.Y, max.Z);
            m_boxCorners[6] = new Vector3(max.X, min.Y, max.Z);
            m_boxCorners[7] = new Vector3(max.X, max.Y, max.Z);
        }

        public WorldOutlineData(WorldOutlineItem owner)
            : base()
        {
            Owner = owner;
        }
    }
}
