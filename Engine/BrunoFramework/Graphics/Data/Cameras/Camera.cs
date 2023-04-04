
using System;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Graphics.Data
{
    public class Camera
    {
        [Flags]
        enum State : short
        {
            None = 0,
            View = 1,
            Projection = 2
        };

        public Vector3 Position
        {
            get
            {
                return m_position;
            }
            set
            {
                m_position = value;
                m_viewDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private Vector3 m_position;

        public Vector3 Target
        {
            get
            {
                return m_target;
            }
            set
            {
                m_target = value;
                m_viewDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private Vector3 m_target;

        public Vector3 Up
        {
            get
            {
                return m_up;
            }
            set
            {
                m_up = value;
                m_viewDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private Vector3 m_up;

        public float NearPlane
        {
            get
            {
                return m_nearPlane;
            }
            set
            {
                if (m_nearPlane == value) return;

                m_nearPlane = value;
                m_projectionDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private float m_nearPlane;

        public float FarPlane
        {
            get
            {
                return m_farPlane;
            }
            set
            {
                if (m_farPlane == value) return;

                m_farPlane = value;
                m_projectionDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private float m_farPlane;
        
        //Radians
        public float FieldOfView
        {
            get
            {
                return m_fov;
            }
            set
            {
                if (m_fov == value) return;

                m_fov = value;
                m_projectionDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private float m_fov;

        public float AspectRatio
        {
            get
            {
                return m_viewport.AspectRatio;
            }
        }

        public Matrix View 
        {
            get
            {
                if (m_viewDirty)
                {
                    m_view = Matrix.CreateLookAt(m_position, m_target, m_up);
                    m_viewDirty = false;
                }
                return m_view;
            }
            set
            {
                m_view = value;
                m_viewDirty = false;
                m_viewProjectionDirty = true;
            }
        }
        private Matrix m_view;

        public Matrix Projection
        {
            get
            {
                if (m_projectionDirty)
                {
                    m_projection = Matrix.CreatePerspectiveFieldOfView(m_fov, m_viewport.AspectRatio, m_nearPlane, m_farPlane);
                    m_projectionDirty = false;
                }
                return m_projection;
            }
            set
            {
                m_projection = value;
                m_projectionDirty = false;
                m_viewProjectionDirty = true;
            }
        }
        private Matrix m_projection;

        public Matrix ViewProjection
        {
            get
            {
                if (m_viewProjectionDirty)
                {
                    m_viewProjection = View * Projection;
                    m_viewProjectionDirty = false;
                }
                return m_viewProjection;
            }
        }
        private Matrix m_viewProjection;

        public Viewport Viewport
        {
            get => m_viewport;
            set
            {
                if (m_viewport == value)
                    return;

                m_viewport = value;

                m_projectionDirty = true;
                m_viewProjectionDirty = true;
            }
        }
        private Viewport m_viewport;

        private bool m_viewDirty, m_projectionDirty, m_viewProjectionDirty;

        private Camera()
        {
        }

        public Camera(Vector3 position, Vector3 target, Vector3 up)
        {
            Position = position;
            Target = target;
            Up = up;

            m_viewDirty = true;
            m_projectionDirty = true;
            m_viewProjectionDirty = true;
        }

        public Camera Clone()
        {
            var clone = new Camera(Position, Target, Up);
            clone.Viewport = Viewport;
            clone.FieldOfView = FieldOfView;
            clone.NearPlane = NearPlane;
            clone.FarPlane = FarPlane;
            return clone;
        }

        public static Camera Empty()
        {
            return new Camera();
        }
    }
}
