
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
        public const float GIZMO_LENGTH = LINE_OFFSET + LINE_LENGTH + CONE_HEIGHT;

        private const float MULTI_AXIS_THICKNESS = 0.05f;
        private const float SINGLE_AXIS_THICKNESS = 0.35f;

        public IAxisGizmoRenderer AxisGizmoTranslationRenderer
        {
            get => m_axisGizmoTranslationRenderer;
            set => m_axisGizmoTranslationRenderer = value;
        }
        private IAxisGizmoRenderer m_axisGizmoTranslationRenderer;

        public IAxisGizmoRenderer AxisGizmoScaleRenderer
        {
            get => m_axisGizmoScaleRenderer;
            set => m_axisGizmoScaleRenderer = value;
        }
        private IAxisGizmoRenderer m_axisGizmoScaleRenderer;

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
        private Vector3 m_currentDelta;

        private static BoundingBox XAxisBox
        {
            get
            {
                var min = new Vector3(LINE_OFFSET, 0.0f, 0.0f);
                var max = new Vector3(GIZMO_LENGTH, SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox YAxisBox
        {
            get
            {
                var min = new Vector3(0.0f, LINE_OFFSET, 0.0f);
                var max = new Vector3(SINGLE_AXIS_THICKNESS, GIZMO_LENGTH, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox ZAxisBox
        {
            get
            {
                var min = new Vector3(0.0f, 0.0f, LINE_OFFSET);
                var max = new Vector3(SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS, GIZMO_LENGTH);

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

        private static BoundingBox XYZBox
        {
            get
            {
                var min = new Vector3(-LINE_LENGTH * 0.25f, -LINE_LENGTH * 0.25f, -LINE_LENGTH * 0.25f);
                var max = new Vector3(LINE_LENGTH * 0.25f, LINE_LENGTH * 0.25f, LINE_LENGTH * 0.25f);

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
            public Matrix m_sceneWorld;
            public Vector2 m_prevMousePosition;
        }
        private SelectionState m_selectionState;

        public event Action<GizmoTransformable, Vector3> OnTranslationChanged;
        public event Action<GizmoTransformable, Vector3, bool> OnScaleChanged;
        public event Action<GizmoTransformable, Matrix> OnRotateChanged;

        public GizmoService(GraphicsDevice graphicsService)
        {
            m_graphicsService = graphicsService;

            InitializeGizmos();
        }

        private void InitializeGizmos()
        {
            m_currentGizmoType = GizmoType.Translation;

            m_axisGizmoTranslationRenderer = new AxisGizmoTranslationRenderer(m_graphicsService);
            m_axisGizmoScaleRenderer = new AxisGizmoScaleRenderer(m_graphicsService);

            m_selectionState = new SelectionState();
            m_selectionState.m_rotationMatrix = Matrix.Identity;
            m_selectionState.m_gizmoWorld = Matrix.Identity;
            m_selectionState.m_axisAlignedWorld = Matrix.Identity;
            m_selectionState.m_screenScaleMatrix = Matrix.Identity;
            m_selectionState.m_sceneWorld = Matrix.Identity;
        }

        public bool Begin(Vector2 mousePosition)
        {
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return false;

            float closestintersection = float.MaxValue;
            Ray ray = ConvertMouseToRay(mousePosition);

            //if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
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
            else if (m_currentGizmoType == GizmoType.Scale)
            {
                intersection = XYZBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestintersection)
                    {
                        m_currentGizmoAxis = GizmoAxis.XYZ;
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
                    {
                        var translationDelta = GetDeltaMovement(mousePosition);

                        translationDelta = Vector3.Transform(translationDelta, m_selectionState.m_rotationMatrix);

                        SetGizmoPosition(translationDelta);

                        if (translationDelta != Vector3.Zero)
                        {
                            if (OnTranslationChanged != null)
                            {
                                OnTranslationChanged(m_selectionState.m_gizmoTransformable, translationDelta);
                            }
                        }
                    }
                    break;
                case GizmoType.Rotation:
                    {
                        var rotationDelta = GetRotationDelta(mousePosition);

                        if (rotationDelta != Matrix.Identity)
                        {
                            if (OnRotateChanged != null)
                            {
                                OnRotateChanged(m_selectionState.m_gizmoTransformable, rotationDelta);
                            }
                        }
                        m_selectionState.m_prevMousePosition = mousePosition;
                    }
                    break;
                case GizmoType.Scale:
                    {
                        var scaleDelta = GetDeltaMovement(mousePosition);

                        if (scaleDelta != Vector3.Zero)
                        {
                            if (OnScaleChanged != null)
                            {
                                OnScaleChanged(m_selectionState.m_gizmoTransformable, scaleDelta, m_currentGizmoAxis == GizmoAxis.XYZ);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            Update();
        }

        private Matrix GetRotationDelta(Vector2 mousePosition)
        {
            float delta = mousePosition.X - m_selectionState.m_prevMousePosition.X;
            delta *= 0.15f;

            Matrix rot = Matrix.Identity;
            rot.Forward = m_selectionState.m_sceneWorld.Forward;
            rot.Up = m_selectionState.m_sceneWorld.Up;
            rot.Right = m_selectionState.m_sceneWorld.Right;

            switch (m_currentGizmoAxis)
            {
                case GizmoAxis.X:
                    rot *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Right, delta);
                    break;
                case GizmoAxis.Y:
                    rot *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Up, delta);
                    break;
                case GizmoAxis.Z:
                    rot *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Forward, delta);
                    break;
            }
            return rot;
        }

        private Vector3 GetDeltaMovement(Vector2 mousePosition)
        {
            Vector3 delta = Vector3.Zero;
            Ray ray = ConvertMouseToRay(mousePosition);

            Matrix transformRotation = Matrix.Invert(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, transformRotation);
            ray.Direction = Vector3.TransformNormal(ray.Direction, transformRotation);

            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;

            switch (m_currentGizmoAxis)
            {
                case GizmoAxis.X:
                case GizmoAxis.XY:
                    {
                        Plane plane = new Plane(Vector3.Forward,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, transformRotation).Z);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * intersection.Value);
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = m_currentGizmoAxis == GizmoAxis.X
                                      ? new Vector3(m_currentDelta.X, 0.0f, 0.0f)
                                      : new Vector3(m_currentDelta.X, m_currentDelta.Y, 0.0f);
                        }
                    }
                    break;
                case GizmoAxis.Y:
                case GizmoAxis.YZ:
                case GizmoAxis.Z:
                    {
                        Plane plane = new Plane(Vector3.Left,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, transformRotation).X);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * intersection.Value);
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            switch (m_currentGizmoAxis)
                            {
                                case GizmoAxis.Y:
                                    delta = new Vector3(0, m_currentDelta.Y, 0);
                                    break;
                                case GizmoAxis.Z:
                                    delta = new Vector3(0, 0, m_currentDelta.Z);
                                    break;
                                default:
                                    delta = new Vector3(0, m_currentDelta.Y, m_currentDelta.Z);
                                    break;
                            }
                        }
                        else
                        {
                            Console.WriteLine("no hubo inter");
                        }
                    }
                    break;
                case GizmoAxis.ZX:
                    {
                        Plane plane = new Plane(Vector3.Down,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, transformRotation).Y);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * intersection.Value);
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = new Vector3(m_currentDelta.X, 0, m_currentDelta.Z);
                        }
                    }
                    break;
                case GizmoAxis.XYZ:
                    {
                        Plane plane = new Plane(Vector3.Up,
                            Vector3.Transform(m_selectionState.m_gizmoPosition, transformRotation).Y);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * intersection.Value);
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = new Vector3(m_currentDelta.X, m_currentDelta.Y, m_currentDelta.Z);
                        }
                    }
                    break;
                default:
                    break;
            }

            return delta;
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

            selectionPosition += translationDelta;

            m_selectionState.m_gizmoPosition = selectionPosition;
            m_selectionState.m_gizmoTransformable.Transform.WorldMatrix.Translation = selectionPosition;
        }

        public void End()
        {
            m_selectionState.m_prevIntersectionPosition = Vector3.Zero;
            m_selectionState.m_intersectionPosition = Vector3.Zero;
            m_currentDelta = Vector3.Zero;
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

            var viewport = m_graphicsService.Viewport;
            nearPoint = viewport.Unproject(nearPoint,
                                                m_camera.Projection,
                                                m_camera.View,
                                                Matrix.Identity);

            farPoint = viewport.Unproject(farPoint,
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

            //Matrix sceneWorld = m_selectionState.m_sceneWorld;
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

            Update();
            switch (m_currentGizmoType)
            {
                case GizmoType.Translation:
                    m_axisGizmoTranslationRenderer.SetWorld(m_selectionState.m_gizmoWorld);
                    m_axisGizmoTranslationRenderer.Render(renderContext);
                    break;
                case GizmoType.Rotation:
                    m_axisGizmoScaleRenderer.SetWorld(m_selectionState.m_gizmoWorld);
                    m_axisGizmoScaleRenderer.Render(renderContext);
                    break;
                case GizmoType.Scale:
                    m_axisGizmoScaleRenderer.SetWorld(m_selectionState.m_gizmoWorld);
                    m_axisGizmoScaleRenderer.Render(renderContext);
                    break;
                default:
                    break;
            }
        }

        public void SetObjectSelected(GizmoTransformable gizmoTransformable)
        {
            m_selectionState.m_gizmoTransformable = gizmoTransformable;

            m_selectionState.m_sceneWorld = gizmoTransformable.Transform.WorldMatrix;
            m_selectionState.m_gizmoPosition = gizmoTransformable.Transform.WorldMatrix.Translation;

            Update();
        }
    }
}
