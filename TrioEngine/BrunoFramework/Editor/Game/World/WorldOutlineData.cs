
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Gizmos;
using BrunoWindows;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineData : PropertyChangedBase, ITransformable
    {
        public string Name
        {
            get; set;
        }

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
    }
}
