
using BrunoFramework.Editor.Timing;
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Game;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public class GizmoService : IGizmoService
    {
        public const float GIZMO_SCREEN_SCALE = 0.1f;
        public const float LINE_LENGTH = 3.0f;
        public const float CONE_HEIGHT = 0.5f;
        public const float CONE_RADIUS = 0.25f;
        public const float LINE_OFFSET = 1.0f;
        public const float GIZMO_LENGTH = LINE_LENGTH + CONE_HEIGHT;

        private const float MULTI_AXIS_THICKNESS = 0.05f;
        private const float SINGLE_AXIS_THICKNESS = 0.35f;

        public AxisGizmoTranslationRenderer AxisGizmoTranslationRenderer
        {
            get => m_axisGizmoTranslationRenderer;
            set => m_axisGizmoTranslationRenderer = value;
        }
        private AxisGizmoTranslationRenderer m_axisGizmoTranslationRenderer;

        public AxisGizmoScaleRenderer AxisGizmoScaleRenderer
        {
            get => m_axisGizmoScaleRenderer;
            set => m_axisGizmoScaleRenderer = value;
        }
        private AxisGizmoScaleRenderer m_axisGizmoScaleRenderer;

        public AxisGizmoRotationRenderer AxisGizmoRotationRenderer
        {
            get => m_axisGizmoRotationRenderer;
            set => m_axisGizmoRotationRenderer = value;
        }
        private AxisGizmoRotationRenderer m_axisGizmoRotationRenderer;

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


        private GameStepTimer m_gameTimer;

        private static BoundingBox XAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(GIZMO_LENGTH, SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox YAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(SINGLE_AXIS_THICKNESS, GIZMO_LENGTH, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private static BoundingBox ZAxisBox
        {
            get
            {
                var min = Vector3.Zero;
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

            public float m_screenScaleFactor, m_invScreenScaleFactor;
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

        public GizmoService(GraphicsDevice graphicsService, GameStepTimer gameStepTimer)
        {
            m_graphicsService = graphicsService;
            m_gameTimer = gameStepTimer;

            InitializeGizmos();
        }

        private void InitializeGizmos()
        {
            m_currentGizmoType = GizmoType.Translation;

            m_axisGizmoTranslationRenderer = new AxisGizmoTranslationRenderer(m_graphicsService);
            m_axisGizmoScaleRenderer = new AxisGizmoScaleRenderer(m_graphicsService);
            m_axisGizmoRotationRenderer = new AxisGizmoRotationRenderer(m_graphicsService, GIZMO_LENGTH);

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

            if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
            {
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
            }
            else if (m_currentGizmoType == GizmoType.Rotation)
            {
                var planeNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };

                GizmoAxis axis = GizmoAxis.None;
                for (int i = 0; i < 3; i++)
                {
                    var plane = new Plane(planeNormals[i], 0);
                    var intersection = ray.Intersects(plane);
                    if (intersection.HasValue)
                    {
                        var positionOnPlane = ray.Position + (ray.Direction * intersection.Value);
                        if (positionOnPlane.Length() > GIZMO_LENGTH)
                        {
                            continue;
                        }
                        else
                        {
                            //var point = GIZMO_LENGTH * Vector3.Normalize(positionOnPlane);
                            //var norm = (positionOnPlane - point).Length();
                            //Console.WriteLine("point = " + point + "; GIZMO_LENGTH = " + GIZMO_LENGTH + "; norm = " + norm)
                            m_selectionState.m_intersectionPosition = GIZMO_LENGTH * Vector3.Normalize(positionOnPlane);

                            //Console.WriteLine("candidate = " + ((GizmoAxis)(i + 1)).ToString());
                            if (intersection.Value < closestintersection)
                            {
                                axis = ((GizmoAxis)(i + 1));
                                //Console.WriteLine("selected = " + m_currentGizmoAxis);
                                closestintersection = intersection.Value;
                            }
                        }
                    }
                }
                if (axis != GizmoAxis.None)
                {
                    m_currentGizmoAxis = axis;
                    Console.WriteLine("selected = " + m_currentGizmoAxis);
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
                            if (m_currentGizmoAxis != GizmoAxis.XYZ)
                            {
                                m_axisGizmoScaleRenderer.UpdateGizmoLength(m_currentGizmoAxis, scaleDelta * m_selectionState.m_invScreenScaleFactor);
                            }
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
            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;
            if (m_selectionState.m_prevMousePosition == Vector2.Zero)
            {
                return Matrix.Identity;
            }

            var planeNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };
            var upNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };
            int planeIndex = ((int)m_currentGizmoAxis) - 1;

            Ray ray = ConvertMouseToRay(mousePosition);

            var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoWorld);

            ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
            ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

            var plane = new Plane(planeNormals[planeIndex], 0);
            var intersection = ray.Intersects(plane);

            float delta = 0.0f;
            if (intersection.HasValue)
            {
                var positionOnPlane = ray.Position + (ray.Direction * intersection.Value);

                var point = GIZMO_LENGTH * Vector3.Normalize(positionOnPlane);
                m_selectionState.m_intersectionPosition = point;
                //var norm = (positionOnPlane - point).Length();

                float acosAngle = MathHelper.Clamp(Vector3.Dot(point, m_selectionState.m_prevIntersectionPosition), -1.0f, 1.0f);
                float angle = MathHelper.Acos(MathHelper.ToRadians(acosAngle));

                var perpendicularVector = Vector3.Cross(m_selectionState.m_prevIntersectionPosition, point);
                perpendicularVector.Normalize();

                //var sign = Vector3.Dot(perpendicularVector, upNormals[planeIndex]) < 0.0f ? 1.0f : -1.0f;
                var pUpDot = Vector3.Dot(perpendicularVector, upNormals[planeIndex]);
                var sign = pUpDot / Math.Abs(pUpDot);

                delta = sign * angle;
                delta *= (float)m_gameTimer.ElapsedTime.TotalSeconds;
                delta *= m_selectionState.m_invScreenScaleFactor;
                Console.WriteLine("delta = " + delta);

            }

            Matrix rotationDelta = Matrix.Identity;
            rotationDelta.Forward = m_selectionState.m_sceneWorld.Forward;
            rotationDelta.Up = m_selectionState.m_sceneWorld.Up;
            rotationDelta.Right = m_selectionState.m_sceneWorld.Right;

            switch (m_currentGizmoAxis)
            {
                case GizmoAxis.X:
                    rotationDelta *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Right, delta);
                    break;
                case GizmoAxis.Y:
                    rotationDelta *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Up, delta);
                    break;
                case GizmoAxis.Z:
                    rotationDelta *= Matrix.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Forward, delta);
                    break;
            }

            return rotationDelta;
        }

        private Vector3 GetDeltaMovement(Vector2 mousePosition)
        {
            Vector3 delta = Vector3.Zero;
            Ray ray = ConvertMouseToRay(mousePosition);

            Matrix transformRotation = Matrix.Invert(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, transformRotation);
            ray.Direction = Vector3.TransformNormal(ray.Direction, transformRotation);

            var gizmoPositionTransformed = Vector3.Transform(m_selectionState.m_gizmoPosition, transformRotation);
            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;

            switch (m_currentGizmoAxis)
            {
                case GizmoAxis.X:
                case GizmoAxis.XY:
                    {
                        Plane plane = new Plane(Vector3.Forward, gizmoPositionTransformed.Z);

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
                    {
                        Plane plane = new Plane(Vector3.Left, gizmoPositionTransformed.X);

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
                                case GizmoAxis.YZ:
                                    delta = new Vector3(0, m_currentDelta.Y, m_currentDelta.Z);
                                    break;
                            }
                        }
                    }

                    break;
                case GizmoAxis.Z:
                    {
                        Plane plane = new Plane(Vector3.Down, gizmoPositionTransformed.Y);

                        float? intersection = ray.Intersects(plane);
                        if (intersection.HasValue)
                        {
                            m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * intersection.Value);
                            if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                            {
                                m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                            }
                            delta = new Vector3(0, 0, m_currentDelta.Z);
                        }
                    }
                    break;
                case GizmoAxis.ZX:
                    {
                        Plane plane = new Plane(Vector3.Down, gizmoPositionTransformed.Y);

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
                        Plane plane = new Plane(Vector3.Up, gizmoPositionTransformed.Y);

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
            m_selectionState.m_prevMousePosition = Vector2.Zero;
            m_currentDelta = Vector3.Zero;

            if (m_currentGizmoType == GizmoType.Scale && m_currentGizmoAxis != GizmoAxis.XYZ)
            {
                m_axisGizmoScaleRenderer.RestoreGizmo(m_currentGizmoAxis, LINE_LENGTH);
            }
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
            var vLength = m_camera.Position - m_selectionState.m_gizmoPosition;

            m_selectionState.m_screenScaleFactor = vLength.Length() * GIZMO_SCREEN_SCALE;
            if (m_selectionState.m_screenScaleFactor < 0.0001f)
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f;
            }
            else
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f / m_selectionState.m_screenScaleFactor;
            }
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
                    {
                        //var rotationCam = Quaternion.CreateFromMatrix((Camera.View));
                        //var world = Matrix.CreateFromQuaternion(rotationCam) * m_selectionState.m_gizmoWorld;
                        var world = m_selectionState.m_gizmoWorld;
                        AxisGizmoRotationRenderer.SetWorld(world);
                        AxisGizmoRotationRenderer.Render(renderContext);
                    }
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
