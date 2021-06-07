
using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System;
using TrioApi.Net.Game;
using TrioApi.Net.Graphics;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Gizmos
{
    public class GizmoService : IGizmoService
    {
        public const float GIZMO_SCREEN_SCALE = 0.1f;
        public const float LINE_LENGTH = 2.0f;
        public const float CONE_HEIGHT = 0.5f;
        public const float CONE_RADIUS = 0.25f;
        public const float LINE_OFFSET = 1.0f;

        private const float MULTI_AXIS_THICKNESS = 0.05f;
        private const float SINGLE_AXIS_THICKNESS = 0.35f;

        public IAxisGizmoRenderer AxisGizmoRenderer
        {
            get => m_axisGizmoRenderer;
            set => m_axisGizmoRenderer = value;
        }
        private IAxisGizmoRenderer m_axisGizmoRenderer;
        public Camera Camera
        {
            get => m_camera;
            set => m_camera = value;
        }
        private Camera m_camera;

        public bool IsActive
        {
            get => m_isActive;
            set => m_isActive = value;
        }
        private bool m_isActive;

        public GizmoType CurrentGizmo 
        { 
            get => m_currentGizmoType; 
            set => m_currentGizmoType = value; 
        }
        private GizmoType m_currentGizmoType;

        public PivotType PivotType
        {
            get => m_pivotType;
            set => m_pivotType = value;
        }
        private PivotType m_pivotType = PivotType.SelectionCenter;

        public GizmoAxis CurrentGizmoAxis
        {
            get => m_currentGizmoAxis;
        }
        private GizmoAxis m_currentGizmoAxis;

        private GraphicsDevice m_graphicsService;

        private static BoundingBox XAxisBox
        {
            get
            {
                var min = new Vector3(LINE_OFFSET, 0.0f, 0.0f);
                var max = new Vector3(LINE_OFFSET + LINE_LENGTH, SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox YAxisBox
        {
            get
            {
                var min = new Vector3(0.0f, LINE_OFFSET, 0.0f);
                var max = new Vector3(SINGLE_AXIS_THICKNESS, LINE_OFFSET + LINE_LENGTH, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox ZAxisBox
        {
            get
            {
                var min = new Vector3(0.0f, 0.0f, LINE_OFFSET);
                var max = new Vector3(SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS, LINE_OFFSET + LINE_LENGTH);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox XZAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(LINE_OFFSET, MULTI_AXIS_THICKNESS, LINE_OFFSET);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox XYBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(LINE_OFFSET, LINE_OFFSET, MULTI_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox YZBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(MULTI_AXIS_THICKNESS, LINE_OFFSET, LINE_OFFSET);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private class SelectionState
        {
            public GizmoTransformable m_gizmoTransformable;
            public Vector3 m_gizmoPosition;

            public float m_screenScaleFactor;
            public Matrix m_screenScaleMatrix;

            public Matrix m_axisAlignedWorld;
            public Matrix m_gizmoWorld;

            public Matrix m_rotationMatrix;

            public Vector3 m_intersectionPosition, m_prevIntersectionPosition;
            //public Vector3 m_delta;
        }
        private SelectionState m_selectionState;

        public event Action<GizmoTransformable, Vector3> OnTranslationChanged;

        public GizmoService(GraphicsDevice graphicsService)
        {
            m_graphicsService = graphicsService;

            InitializeGizmos();
        }

        private void InitializeGizmos()
        {
            m_currentGizmoType = GizmoType.Translation;

            m_axisGizmoRenderer = new AxisGizmoRenderer(m_graphicsService);

            m_selectionState = new SelectionState();
            m_selectionState.m_rotationMatrix = Matrix.Identity;
            m_selectionState.m_gizmoWorld = Matrix.Identity;
            m_selectionState.m_axisAlignedWorld = Matrix.Identity;
            m_selectionState.m_screenScaleMatrix = Matrix.Identity;
        }

        public bool Begin(Vector2 mousePosition)
        {
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return false;

            float closestintersection = float.MaxValue;
            Ray ray = ConvertMouseToRay(mousePosition);

            if (m_currentGizmoType == GizmoType.Translation)
            {
                var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoWorld);

                ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
                ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);
            }

            float? intersection = XAxisBox.Intersects(ray);
            if (intersection.HasValue)
            {
                if (intersection.Value < closestintersection)
                {
                    m_currentGizmoAxis = GizmoAxis.X;
                    closestintersection = intersection.Value;
                }
            }
            intersection = YAxisBox.Intersects(ray);
            if (intersection.HasValue)
            {
                if (intersection.Value < closestintersection)
                {
                    m_currentGizmoAxis = GizmoAxis.Y;
                    closestintersection = intersection.Value;
                }
            }

            intersection = ZAxisBox.Intersects(ray);
            if (intersection.HasValue)
            {
                if (intersection.Value < closestintersection)
                {
                    m_currentGizmoAxis = GizmoAxis.Z;
                    closestintersection = intersection.Value;
                }
            }

            if (m_currentGizmoType == GizmoType.Translation)
            {
                if (closestintersection >= float.MaxValue)
                    closestintersection = float.MinValue;

                intersection = XYBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value > closestintersection)
                    {
                        m_currentGizmoAxis = GizmoAxis.XY;
                        closestintersection = intersection.Value;
                    }
                }

                intersection = XZAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value > closestintersection)
                    {
                        m_currentGizmoAxis = GizmoAxis.ZX;
                        closestintersection = intersection.Value;
                    }
                }
                intersection = YZBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value > closestintersection)
                    {
                        m_currentGizmoAxis = GizmoAxis.YZ;
                        closestintersection = intersection.Value;
                    }
                }
            }

            if (closestintersection >= float.MaxValue || closestintersection <= float.MinValue)
                m_currentGizmoAxis = GizmoAxis.None;

            return m_currentGizmoAxis != GizmoAxis.None;
        }

        public void UpdateGizmo(Vector2 mousePosition)
        {
            switch (m_currentGizmoType)
            {
                case GizmoType.Translation:
                    UpdateTranslation(mousePosition);
                    break;
                case GizmoType.Rotation:
                    break;
                case GizmoType.Scale:
                    break;
                default:
                    break;
            }
            Update();
        }

        private void UpdateTranslation(Vector2 mousePosition)
        {
            Vector3 delta = Vector3.Zero;
            Vector3 translationDelta = Vector3.Zero;
            Ray ray = ConvertMouseToRay(mousePosition);

            Matrix transform = Matrix.Invert(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, transform);
            ray.Direction = Vector3.TransformNormal(ray.Direction, transform);

            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;
            //Console.WriteLine($"current axis = {m_currentGizmoAxis}");

            switch (m_currentGizmoAxis)
            {
                case GizmoAxis.X:
                case GizmoAxis.XY:
                    {
                        Plane plane = new Plane(Vector3.Forward,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, Matrix.Invert(m_selectionState.m_rotationMatrix)).Z);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = (ray.Position + (ray.Direction * intersection.Value));
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                delta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = m_currentGizmoAxis == GizmoAxis.X
                                      ? new Vector3(delta.X, 0.0f, 0.0f)
                                      : new Vector3(delta.X, delta.Y, 0.0f);
                        }
                    }
                    break;
                case GizmoAxis.Y:
                case GizmoAxis.YZ:
                case GizmoAxis.Z:
                    {
                        Plane plane = new Plane(Vector3.Left,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, Matrix.Invert(m_selectionState.m_rotationMatrix)).X);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = (ray.Position + (ray.Direction * intersection.Value));
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                delta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            switch (m_currentGizmoAxis)
                            {
                                case GizmoAxis.Y:
                                    delta = new Vector3(0, delta.Y, 0);
                                    break;
                                case GizmoAxis.Z:
                                    delta = new Vector3(0, 0, delta.Z);
                                    break;
                                default:
                                    delta = new Vector3(0, delta.Y, delta.Z);
                                    break;
                            }
                        }
                    }
                    break;
                case GizmoAxis.ZX:
                    {
                        Plane plane = new Plane(Vector3.Down,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, Matrix.Invert(m_selectionState.m_rotationMatrix)).Y);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = (ray.Position + (ray.Direction * intersection.Value));
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                delta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = new Vector3(delta.X, 0, delta.Z);

                        }
                    }
                    break;
                default:
                    break;
            }

            delta = Vector3.Transform(delta, m_selectionState.m_rotationMatrix);
            translationDelta = delta;


            if (translationDelta != Vector3.Zero)
            {
                if (OnTranslationChanged != null)
                {
                    OnTranslationChanged(m_selectionState.m_gizmoTransformable, translationDelta);
                }
            SetGizmoPosition(translationDelta);
            }
            else
            {
                Console.WriteLine("no delta");
            }
        }

        private void SetGizmoPosition(Vector3 translationDelta)
        {
            Vector3 selectionPosition = Vector3.Zero;
            switch (m_pivotType)
            {
                case PivotType.ObjectCenter:
                    break;
                case PivotType.SelectionCenter:
                    selectionPosition = GetSelectionCenter();
                    break;
                case PivotType.WorldOrigin:
                    break;
                default:
                    break;
            }
            selectionPosition = selectionPosition + translationDelta;

            m_selectionState.m_gizmoPosition = selectionPosition;
            m_selectionState.m_gizmoTransformable.Transform.WorldMatrix.Translation = selectionPosition;
        }

        public void End()
        {
            m_selectionState.m_prevIntersectionPosition = Vector3.Zero;
            m_selectionState.m_intersectionPosition = Vector3.Zero;
            Update();
        }

        private Vector3 GetSelectionCenter()
        {
            return m_selectionState.m_gizmoTransformable.Transform.WorldMatrix.Translation;
        }

        private Ray ConvertMouseToRay(Vector2 mousePosition)
        {
            Vector3 nearPoint = new Vector3(mousePosition, 0.0f);
            Vector3 farPoint = new Vector3(mousePosition, 1.0f);

            nearPoint = m_graphicsService.Viewport.Unproject(nearPoint,
                                                      m_camera.Projection,
                                                     m_camera.View,
                                                     Matrix.Identity);

            farPoint = m_graphicsService.Viewport.Unproject(farPoint,
                                                      m_camera.Projection,
                                                     m_camera.View,
                                                    Matrix.Identity);

            Vector3 direction = farPoint - nearPoint;
            direction.Normalize();

            return new Ray(nearPoint, direction);
        }

        private void Update()
        {
            Vector3 vLength = m_camera.Position - m_selectionState.m_gizmoPosition;

            m_selectionState.m_screenScaleFactor = vLength.Length() * GIZMO_SCREEN_SCALE;
            m_selectionState.m_screenScaleMatrix = Matrix.CreateScale(new Vector3(m_selectionState.m_screenScaleFactor));

            Matrix sceneWorld = Matrix.Identity;
            m_selectionState.m_axisAlignedWorld = m_selectionState.m_screenScaleMatrix * Matrix.CreateWorld(m_selectionState.m_gizmoPosition, sceneWorld.Forward, sceneWorld.Up);

            m_selectionState.m_gizmoWorld = m_selectionState.m_axisAlignedWorld;
            m_selectionState.m_rotationMatrix.Forward = sceneWorld.Forward;
            m_selectionState.m_rotationMatrix.Up = sceneWorld.Up;
            m_selectionState.m_rotationMatrix.Right = sceneWorld.Right;
        }

        public void Render(RenderContext renderContext)
        {
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return;

            m_axisGizmoRenderer.SetWorld(m_selectionState.m_gizmoWorld);
            m_axisGizmoRenderer.Render(renderContext);
        }

        public void SetObjectSelected(GizmoTransformable gizmoTransformable)
        {
            m_selectionState.m_gizmoTransformable = gizmoTransformable;

            m_selectionState.m_gizmoPosition = gizmoTransformable.Transform.WorldMatrix.Translation;

            Update();
        }
    }
}
