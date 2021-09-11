
using BrunoFramework.Editor.Timing;
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Maths;
using BrunoApi.Net.Graphics.Core;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public class GizmoService : IGizmoService
    {
        public const float GIZMO_SCREEN_SCALE = 0.1f;
        public const float GIZMO_SIZE_CLIP_SPACE = 2.3f;
        public const float CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS = 60.0f;

        private const float MULTI_AXIS_THICKNESS = 0.05f;
        private const float SINGLE_AXIS_THICKNESS = 0.35f;
        public GizmoConfig m_gizmoConfig;

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

        private GraphicsDevice m_graphicsDevice;
        private readonly ObjectSelector m_objectSelector;
        private Vector3 m_currentDelta;

        private ColorRGBA8[] m_activeAxisColors;
        private ColorRGBA8[] m_axisColors;
        private ColorRGBA8 m_selectedAxisColor;
        private GameStepTimer m_gameTimer;
        private Camera m_sceneGizmoCamera;

        private Vector3[] unaryDirections = new Vector3[] { Vector3.UnitX, Vector3.UnitY, Vector3.UnitZ };

        private BoundingBox XAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(m_gizmoConfig.GIZMO_LENGTH, SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox YAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(SINGLE_AXIS_THICKNESS, m_gizmoConfig.GIZMO_LENGTH, SINGLE_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox ZAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(SINGLE_AXIS_THICKNESS, SINGLE_AXIS_THICKNESS, m_gizmoConfig.GIZMO_LENGTH);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox XZAxisBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(m_gizmoConfig.LINE_OFFSET, MULTI_AXIS_THICKNESS, m_gizmoConfig.LINE_OFFSET);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox XYBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(m_gizmoConfig.LINE_OFFSET, m_gizmoConfig.LINE_OFFSET, MULTI_AXIS_THICKNESS);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox YZBox
        {
            get
            {
                var min = Vector3.Zero;
                var max = new Vector3(MULTI_AXIS_THICKNESS, m_gizmoConfig.LINE_OFFSET, m_gizmoConfig.LINE_OFFSET);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox XYZBox
        {
            get
            {
                var min = new Vector3(-m_gizmoConfig.LINE_LENGTH * 0.25f, -m_gizmoConfig.LINE_LENGTH * 0.25f, -m_gizmoConfig.LINE_LENGTH * 0.25f);
                var max = new Vector3(m_gizmoConfig.LINE_LENGTH * 0.25f, m_gizmoConfig.LINE_LENGTH * 0.25f, m_gizmoConfig.LINE_LENGTH * 0.25f);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private class SelectionState
        {
            public Vector3 m_gizmoPosition;

            public float m_screenScaleFactor, m_invScreenScaleFactor;
            public Matrix m_screenScaleMatrix;

            public Matrix m_gizmoWorld;

            public Matrix m_rotationMatrix;

            public Vector3 m_intersectionPosition, m_prevIntersectionPosition;
            public Vector2 m_prevMousePosition;

            public Plane m_currentGizmoPlane;
        }
        private SelectionState m_selectionState;

        public event Action<ITransformableObject, Vector3> OnTranslationChanged;
        public event Action<ITransformableObject, Vector3, bool> OnScaleChanged;
        public event Action<ITransformableObject, Quaternion> OnRotateChanged;

        public GizmoService(GraphicsDevice graphicsService, ObjectSelector objectSelector, GameStepTimer gameStepTimer)
        {
            m_graphicsDevice = graphicsService;
            m_objectSelector = objectSelector;
            m_gameTimer = gameStepTimer;

            InitializeGizmos();
            m_sceneGizmoCamera = Camera.Empty();
            m_sceneGizmoCamera.FarPlane = 10.0f;
            m_sceneGizmoCamera.NearPlane = 1.0f;
        }

        private void InitializeGizmos()
        {
            m_currentGizmoType = GizmoType.Translation;
            m_gizmoConfig = new GizmoConfig();
            m_axisColors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };
            m_activeAxisColors = new ColorRGBA8[] { ColorRGBA8.Red, ColorRGBA8.Green, ColorRGBA8.Blue };

            m_selectedAxisColor = ColorRGBA8.Yellow;
            var gizmoConfig = new GizmoConfig();

            m_axisGizmoTranslationRenderer = new AxisGizmoTranslationRenderer(m_graphicsDevice, m_axisColors, gizmoConfig);
            m_axisGizmoScaleRenderer = new AxisGizmoScaleRenderer(m_graphicsDevice, m_axisColors, gizmoConfig);
            m_axisGizmoRotationRenderer = new AxisGizmoRotationRenderer(m_graphicsDevice, m_axisColors, gizmoConfig);

            m_selectionState = new SelectionState();
            m_selectionState.m_rotationMatrix = Matrix.Identity;
            m_selectionState.m_gizmoWorld = Matrix.Identity;
            m_selectionState.m_screenScaleMatrix = Matrix.Identity;
        }

        private GizmoAxis GetAxis(Vector2 mousePosition)
        {
            var closestintersection = float.MaxValue;
            var ray = ConvertMousePositionToRay(mousePosition);

            var selectedAxis = GizmoAxis.None;

            var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoWorld);

            ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
            ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

            if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
            {
                float? intersection = XAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestintersection)
                    {
                        selectedAxis = GizmoAxis.X;
                        closestintersection = intersection.Value;
                    }
                }
                intersection = YAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestintersection)
                    {
                        selectedAxis = GizmoAxis.Y;
                        closestintersection = intersection.Value;
                    }
                }

                intersection = ZAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestintersection)
                    {
                        selectedAxis = GizmoAxis.Z;
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
                            selectedAxis = GizmoAxis.XY;
                            closestintersection = intersection.Value;
                        }
                    }

                    intersection = XZAxisBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value > closestintersection)
                        {
                            selectedAxis = GizmoAxis.XZ;
                            closestintersection = intersection.Value;
                        }
                    }
                    intersection = YZBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value > closestintersection)
                        {
                            selectedAxis = GizmoAxis.YZ;
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
                            selectedAxis = GizmoAxis.XYZ;
                            closestintersection = intersection.Value;
                        }
                    }
                }
            }
            else if (m_currentGizmoType == GizmoType.Rotation)
            {
                var planeNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };

                var currentPointOnPlane = Vector3.Zero;
                for (int i = 0; i < 3; i++)
                {
                    var plane = new Plane(planeNormals[i], 0);
                    var intersection = ray.Intersects(plane);
                    if (intersection.HasValue)
                    {
                        var positionOnPlane = ray.Position + (ray.Direction * intersection.Value);
                        if (positionOnPlane.Length() > m_gizmoConfig.GIZMO_LENGTH)
                        {
                            continue;
                        }
                        else
                        {
                            if (intersection.Value < closestintersection)
                            {
                                m_selectionState.m_intersectionPosition = Vector3.Normalize(positionOnPlane);
                                m_selectionState.m_prevIntersectionPosition = Vector3.Zero;
                                currentPointOnPlane = positionOnPlane;
                                selectedAxis = ((GizmoAxis)(i + 1));
                                //Console.WriteLine("selected = " + selectedAxis);
                                closestintersection = intersection.Value;
                            }
                        }
                    }
                }
                if (selectedAxis != GizmoAxis.None)
                {
                    if (currentPointOnPlane.Length() < m_gizmoConfig.GIZMO_LENGTH * 0.8f) selectedAxis = GizmoAxis.XYZ;
                }
            }

            if (closestintersection >= float.MaxValue || closestintersection <= float.MinValue)
                selectedAxis = GizmoAxis.None;

            return selectedAxis;
        }

        public bool Begin(Vector2 mousePosition)
        {
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return false;

            var selectedAxis = GetAxis(mousePosition);
            m_currentGizmoAxis = selectedAxis;

            SetGizmoHandlePlaneFor(selectedAxis, mousePosition);

            m_currentDelta = Vector3.Zero;
            m_selectionState.m_prevIntersectionPosition = Vector3.Zero;

            return m_currentGizmoAxis != GizmoAxis.None;
        }

        private void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, Vector2 mousePosition)
        {
            var ray = ConvertMousePositionToRay(mousePosition);

            var toLocal = Matrix.Transpose(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, toLocal);
            ray.Direction = Vector3.TransformNormal(ray.Direction, toLocal);

            var gizmoPositionInLocal = Vector3.Transform(m_selectionState.m_gizmoPosition, toLocal);
            var plane = new Plane();

            switch (selectedAxis)
            {
                case GizmoAxis.XY:
                    plane = new Plane(Vector3.Forward, gizmoPositionInLocal.Z);
                    break;
                case GizmoAxis.YZ:
                    plane = new Plane(Vector3.Left, gizmoPositionInLocal.X);
                    break;
                case GizmoAxis.XZ:
                    plane = new Plane(Vector3.Down, gizmoPositionInLocal.Y);
                    break;

                case GizmoAxis.X:
                case GizmoAxis.Y:
                case GizmoAxis.Z:
                    {
                        var cameraToGizmo = m_selectionState.m_gizmoPosition - m_camera.Position;
                        cameraToGizmo.Normalize();
                        cameraToGizmo = Vector3.TransformNormal(cameraToGizmo, toLocal);

                        int axisIndex = (int)selectedAxis - 1;
                        var perpendicularRayVector = Vector3.Cross(unaryDirections[axisIndex], cameraToGizmo);
                        perpendicularRayVector = Vector3.Cross(unaryDirections[axisIndex], perpendicularRayVector);
                        perpendicularRayVector.Normalize();
                        var gizmoPositionToLocal = Vector3.Transform(m_selectionState.m_gizmoPosition, toLocal);
                        var newD = Vector3.Dot(perpendicularRayVector, gizmoPositionToLocal);

                        plane = new Plane(perpendicularRayVector, newD);
                    }
                    break;

                case GizmoAxis.XYZ:
                    {
                        var cameraToGizmo = m_camera.Position - m_selectionState.m_gizmoPosition;
                        cameraToGizmo = Vector3.TransformNormal(cameraToGizmo, toLocal);

                        float zCamera = cameraToGizmo.Length();
                        cameraToGizmo.Normalize();

                        plane = new Plane(cameraToGizmo, zCamera);
                    }
                    break;
            }

            //Console.WriteLine(string.Format("selected plane normal = {0}; Axis = {1}", plane.Normal, selectedAxis.ToString()));
            m_selectionState.m_currentGizmoPlane = plane;
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
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    OnTranslationChanged(m_objectSelector.SelectedObjects[i], translationDelta);
                                }
                            }
                        }
                    }
                    break;
                case GizmoType.Rotation:
                    {
                        var rotationDelta = GetRotationDelta(mousePosition);

                        if (rotationDelta != Quaternion.Identity)
                        {
                            if (OnRotateChanged != null)
                            {
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    OnRotateChanged(m_objectSelector.SelectedObjects[i], rotationDelta);
                                }
                            }
                        }
                    }
                    break;
                case GizmoType.Scale:
                    {
                        var scaleDelta = GetDeltaMovement(mousePosition);

                        if (scaleDelta != Vector3.Zero)
                        {
                            if (m_currentGizmoAxis != GizmoAxis.XYZ)
                            {
                                m_axisGizmoScaleRenderer.UpdateGizmoLength(m_currentGizmoAxis, scaleDelta);
                            }
                            if (OnScaleChanged != null)
                            {
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    OnScaleChanged(m_objectSelector.SelectedObjects[i], scaleDelta, m_currentGizmoAxis == GizmoAxis.XYZ);
                                }
                            }
                        }
                    }
                    break;
                default:
                    break;
            }

            Update();
        }

        public void SetGizmoAxisOverMousePosition(Vector2 mousePosition)
        {
            var selectedAxis = GetAxis(mousePosition);
            if (selectedAxis == GizmoAxis.None)
            {
                m_activeAxisColors[0] = m_axisColors[0];
                m_activeAxisColors[1] = m_axisColors[1];
                m_activeAxisColors[2] = m_axisColors[2];
            }
            else if (selectedAxis == GizmoAxis.XYZ)
            {
                m_activeAxisColors[0] = m_selectedAxisColor;
                m_activeAxisColors[1] = m_selectedAxisColor;
                m_activeAxisColors[2] = m_selectedAxisColor;
            }
            else if (selectedAxis == GizmoAxis.X || selectedAxis == GizmoAxis.Y || selectedAxis == GizmoAxis.Z)
            {
                int axisIndex = ((int)selectedAxis) - 1;
                m_activeAxisColors[axisIndex] = m_selectedAxisColor;
                m_activeAxisColors[(axisIndex + 1) % 3] = m_axisColors[(axisIndex + 1) % 3];
                m_activeAxisColors[(axisIndex + 2) % 3] = m_axisColors[(axisIndex + 2) % 3];
            }
            else if (selectedAxis == GizmoAxis.XY)
            {
                m_activeAxisColors[0] = m_selectedAxisColor;
                m_activeAxisColors[1] = m_selectedAxisColor;
                m_activeAxisColors[2] = m_axisColors[2];
            }
            else if (selectedAxis == GizmoAxis.YZ)
            {
                m_activeAxisColors[0] = m_axisColors[0];
                m_activeAxisColors[1] = m_selectedAxisColor;
                m_activeAxisColors[2] = m_selectedAxisColor;
            }
            else if (selectedAxis == GizmoAxis.XZ)
            {
                m_activeAxisColors[0] = m_selectedAxisColor;
                m_activeAxisColors[1] = m_axisColors[1];
                m_activeAxisColors[2] = m_selectedAxisColor;
            }
            switch (m_currentGizmoType)
            {
                case GizmoType.Translation:
                    {
                        m_axisGizmoTranslationRenderer.SetColors(m_activeAxisColors);
                    }
                    break;
                case GizmoType.Scale:
                    {
                        m_axisGizmoScaleRenderer.SetColors(m_activeAxisColors);
                    }
                    break;
                case GizmoType.Rotation:
                    {
                        m_axisGizmoRotationRenderer.SetColors(m_activeAxisColors);
                    }
                    break;
            }
        }

        private Quaternion GetRotationDelta(Vector2 mousePosition)
        {
            if (m_selectionState.m_prevIntersectionPosition == Vector3.Zero)
            {
                m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;
                return Quaternion.Identity;
            }
            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;

            Quaternion rotationDelta = Quaternion.Identity;
            if (m_currentGizmoAxis == GizmoAxis.XYZ)
            {
                if (m_selectionState.m_prevMousePosition != Vector2.Zero)
                {
                    var gizmoScreenPosition = GetScreenPosition(m_selectionState.m_gizmoPosition);
                    var gizmoScreenPosition2 = GetScreenPosition(m_selectionState.m_gizmoPosition + Vector3.Right * m_gizmoConfig.GIZMO_LENGTH);
                    var length = 3.0f * (gizmoScreenPosition2 - gizmoScreenPosition).Length() / MathHelper.TwoPi;
                    var deltaAngles = new Vector2(1.0f / length);

                    var mouseVelocity = new Vector2(mousePosition.X - m_selectionState.m_prevMousePosition.X, mousePosition.Y - m_selectionState.m_prevMousePosition.Y);

                    var angles = mouseVelocity * deltaAngles;

                    rotationDelta = Quaternion.CreateFromYawPitchRoll(angles.X, 0, 0) * Quaternion.CreateFromYawPitchRoll(0, angles.Y, 0);
                }
            }
            else
            {
                var planeNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };
                var upNormals = new Vector3[] { Vector3.Right, Vector3.Up, Vector3.Forward };
                int planeIndex = ((int)m_currentGizmoAxis) - 1;

                var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoWorld);

                Ray ray = ConvertMousePositionToRay(mousePosition);
                ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
                ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

                var plane = new Plane(planeNormals[planeIndex], 0);
                var intersection = ray.Intersects(plane);

                float delta = 0.0f;
                if (intersection.HasValue)
                {
                    var positionOnPlane = ray.Position + (ray.Direction * intersection.Value);

                    var newIntersectionPoint = Vector3.Normalize(positionOnPlane);
                    m_selectionState.m_intersectionPosition = newIntersectionPoint;

                    float dotP = Vector3.Dot(newIntersectionPoint, m_selectionState.m_prevIntersectionPosition);
                    float acosAngle = MathHelper.Clamp(dotP, -1.0f, 1.0f);
                    float angle = MathHelper.Acos(acosAngle);

                    var perpendicularVector = Vector3.Cross(m_selectionState.m_prevIntersectionPosition, newIntersectionPoint);
                    perpendicularVector.Normalize();

                    var sign = Vector3.Dot(perpendicularVector, upNormals[planeIndex]) < 0.0f ? -1.0f : 1.0f;

                    delta = sign * angle;
                    //delta *= (float)m_gameTimer.ElapsedTime.TotalSeconds;
                    //delta *= m_selectionState.m_invScreenScaleFactor;
                    //delta = NaN; acosAngle = 0.9999999; dotP = 0.9999999; point= {X = 0; Y = 0.4902804; Z = 0.8715647}

                    Console.WriteLine("delta = " + delta + "; acosAngle = " + acosAngle + "; dotP = " + dotP + "; point= " + newIntersectionPoint);
                }

                switch (m_currentGizmoAxis)
                {
                    case GizmoAxis.X:
                        rotationDelta *= Quaternion.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Right, delta);
                        break;
                    case GizmoAxis.Y:
                        rotationDelta *= Quaternion.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Up, delta);
                        break;
                    case GizmoAxis.Z:
                        rotationDelta *= Quaternion.CreateFromAxisAngle(m_selectionState.m_rotationMatrix.Forward, delta);
                        break;
                }
            }

            m_selectionState.m_prevMousePosition = mousePosition;
            return rotationDelta;
        }

        float? IntersectRayPlane(Ray ray, Plane plane)
        {
            float numer = Vector3.Dot(plane.Normal, ray.Position) + plane.D;
            float denom = Vector3.Dot(plane.Normal, ray.Direction);

            if (Math.Abs(denom) < float.Epsilon)
            {
                return null;
            }
            return new float?(-(numer / denom));
        }

        private Vector3 GetDeltaMovement(Vector2 mousePosition)
        {
            Vector3 delta = Vector3.Zero;
            var ray = ConvertMousePositionToRay(mousePosition);

            var toLocal = Matrix.Transpose(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, toLocal);
            ray.Direction = Vector3.TransformNormal(ray.Direction, toLocal);

            SetGizmoHandlePlaneFor(m_currentGizmoAxis, mousePosition);

            var plane = m_selectionState.m_currentGizmoPlane;
            //var intersection = ray.Intersects(plane);
            var intersection = IntersectRayPlane(ray, plane);

            if (intersection.HasValue)
            {
                m_selectionState.m_intersectionPosition = ray.Position + (ray.Direction * Math.Abs(intersection.Value));
                if (m_selectionState.m_prevIntersectionPosition != Vector3.Zero)
                {
                    m_currentDelta = m_selectionState.m_intersectionPosition - m_selectionState.m_prevIntersectionPosition;
                }

                if (m_currentGizmoAxis == GizmoAxis.X || m_currentGizmoAxis == GizmoAxis.XY || m_currentGizmoAxis == GizmoAxis.XZ || m_currentGizmoAxis == GizmoAxis.XYZ)
                {
                    delta.X = m_currentDelta.X;
                }
                if (m_currentGizmoAxis == GizmoAxis.Y || m_currentGizmoAxis == GizmoAxis.XY || m_currentGizmoAxis == GizmoAxis.YZ || m_currentGizmoAxis == GizmoAxis.XYZ)
                {
                    delta.Y = m_currentDelta.Y;
                }
                if (m_currentGizmoAxis == GizmoAxis.Z || m_currentGizmoAxis == GizmoAxis.XZ || m_currentGizmoAxis == GizmoAxis.YZ || m_currentGizmoAxis == GizmoAxis.XYZ)
                {
                    delta.Z = m_currentDelta.Z;
                }
            }

            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;

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
        }

        public void End()
        {
            m_selectionState.m_prevIntersectionPosition = Vector3.Zero;
            m_selectionState.m_intersectionPosition = Vector3.Zero;
            m_selectionState.m_prevMousePosition = Vector2.Zero;
            m_currentDelta = Vector3.Zero;

            if (m_currentGizmoType == GizmoType.Scale && m_currentGizmoAxis != GizmoAxis.XYZ)
            {
                m_axisGizmoScaleRenderer.RestoreGizmo(m_currentGizmoAxis, m_gizmoConfig.LINE_LENGTH);
            }
            Update();
        }

        private Vector3 GetSelectionCenter()
        {
            if (m_objectSelector.SelectedObjects.Count == 0)
            {
                return Vector3.Zero;
            }

            var center = Vector3.Zero;
            var objectCount = m_objectSelector.SelectedObjects.Count;
            for (int i = 0; i < objectCount; i++)
            {
                center += m_objectSelector.SelectedObjects[i].WorldMatrix.Translation;
            }
            return center / objectCount;
        }

        private Ray ConvertMousePositionToRay(Vector2 mousePosition)
        {
            Vector3 nearPoint = new Vector3(mousePosition, 0.0f);
            Vector3 farPoint = new Vector3(mousePosition, 1.0f);

            var viewport = m_graphicsDevice.Viewport;
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

        private Vector2 GetScreenPosition(Vector3 worldPosition)
        {
            var viewport = m_graphicsDevice.Viewport;

            var point = viewport.Project(worldPosition,
                                                m_camera.Projection,
                                                m_camera.View,
                                                Matrix.Identity);

            return new Vector2(point.X, point.Y);
        }

        private float GetSegmentLengthClipSpace(Vector3 start, Vector3 end)
        {
            var viewport = m_graphicsDevice.Viewport;

            var startOfSegment = start;
            var mvp = m_selectionState.m_gizmoWorld * m_camera.ViewProjection;

            startOfSegment=viewport.Project(startOfSegment, m_camera.Projection, m_camera.View, m_selectionState.m_gizmoWorld);
            //startOfSegment = Vector3.Transform(startOfSegment, mvp);
            //if ((float)Math.Abs(startOfSegment.W) > 0.0001f) // check for axis aligned with camera direction
            //{
            //    startOfSegment.X *= 1.0f / startOfSegment.W;
            //    startOfSegment.Y *= 1.0f / startOfSegment.W;
            //    startOfSegment.Z *= 1.0f / startOfSegment.W;
            //    startOfSegment.W *= 1.0f / startOfSegment.W;
            //}

            var endOfSegment = end;
            //endOfSegment = Vector3.Transform(endOfSegment, mvp);
            endOfSegment = viewport.Project(endOfSegment, m_camera.Projection, m_camera.View, m_selectionState.m_gizmoWorld);
            //if ((float)Math.Abs(endOfSegment.W) > 0.0001f) // check for axis aligned with camera direction
            //{
            //    endOfSegment.X *= 1.0f / endOfSegment.W;
            //    endOfSegment.Y *= 1.0f / endOfSegment.W;
            //    endOfSegment.Z *= 1.0f / endOfSegment.W;
            //    endOfSegment.W *= 1.0f / endOfSegment.W;
            //}

            var clipSpaceAxis = endOfSegment - startOfSegment;
            clipSpaceAxis.X = (clipSpaceAxis.X - viewport.X) * 2.0f / viewport.Width-1.0f;
            clipSpaceAxis.Y = (clipSpaceAxis.Y - viewport.Y) * 2.0f / viewport.Height - 1.0f;
            //vector.X = (vector.X + 1f) * 0.5f * (float)this.Width + (float)this.X;
            //vector.Y = (-vector.Y + 1f) * 0.5f * (float)this.Height + (float)this.Y;

            clipSpaceAxis.Y /= m_camera.AspectRatio;

            return (float)Math.Sqrt(clipSpaceAxis.X * clipSpaceAxis.X + clipSpaceAxis.Y * clipSpaceAxis.Y);
        }

        private Vector3 ToClipSpace(Vector2 screenPosition, Viewport viewport)
        {
            Vector3 result = Vector3.Zero;

            result.X = (screenPosition.X - viewport.X) * 2.0f / viewport.Width - 1.0f;
            result.Y = -((screenPosition.Y - viewport.Y) * 2.0f / viewport.Height - 1.0f);
            return result;
        }

        private void Update()
        {
            if (m_camera == null) return;

            var gizmoPositionViewSpace = Vector3.Transform(m_selectionState.m_gizmoPosition, m_camera.View);

            m_selectionState.m_screenScaleFactor = Math.Abs(gizmoPositionViewSpace.Z) * GIZMO_SCREEN_SCALE;
            if (m_selectionState.m_screenScaleFactor < 0.0001f)
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f;
            }
            else
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f / m_selectionState.m_screenScaleFactor;
            }
            //m_selectionState.m_screenScaleMatrix = Matrix.Identity;
            m_selectionState.m_screenScaleMatrix = Matrix.CreateScale(new Vector3(m_selectionState.m_screenScaleFactor));

            if (m_objectSelector.SelectedObjects.Count < 1)
            {
                return;
            }
            m_selectionState.m_rotationMatrix = Matrix.Identity;

            if (m_currentGizmoType == GizmoType.Translation ||
                m_currentGizmoType == GizmoType.Rotation)
            {
                var world = Matrix.Identity;
                m_selectionState.m_gizmoWorld = m_selectionState.m_screenScaleMatrix *
                                                        Matrix.CreateWorld(m_selectionState.m_gizmoPosition, world.Forward, world.Up);

                m_selectionState.m_rotationMatrix.Forward = world.Forward;
                m_selectionState.m_rotationMatrix.Up = world.Up;
                m_selectionState.m_rotationMatrix.Right = world.Right;
            }
            else
            {
                var selectedObject = m_objectSelector.SelectedObjects[0];
                var localRotationMatrix = selectedObject.WorldMatrix;
                var localForward = localRotationMatrix.Forward;
                var localUp = localRotationMatrix.Up;

                localForward.Normalize();
                var localRight = Vector3.Cross(localForward, localUp);
                localUp = Vector3.Cross(localRight, localForward);
                localRight.Normalize();
                localUp.Normalize();

                m_selectionState.m_gizmoWorld = m_selectionState.m_screenScaleMatrix *
                                                        Matrix.CreateWorld(m_selectionState.m_gizmoPosition, localForward, localUp);

                m_selectionState.m_rotationMatrix.Forward = localForward;
                m_selectionState.m_rotationMatrix.Up = localUp;
                m_selectionState.m_rotationMatrix.Right = localRight;
            }
            //const float GIZMO_SIZE_IN_PIXELS = 60.0f;
            //var originWS = m_selectionState.m_gizmoPosition;
            //var viewport = m_graphicsDevice.Viewport;

            //var originSS = viewport.Project(originWS, m_camera.Projection, m_camera.View, Matrix.Identity);

            ////var viewInverse = Matrix.Invert(m_camera.View);
            ////var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoWorld);
            ////var rightInverseNormalized = viewInverse.Right;
            ////rightInverseNormalized.Normalize();
            ////var rightViewInverse = Vector3.TransformNormal(rightInverseNormalized, gizmoWorldInverse);

            //var targetRightSS = viewport.Project(originWS + Vector3.UnitX, m_camera.Projection, m_camera.View, Matrix.Identity);

            //var directionRight = (targetRightSS - originSS).ToVector2();
            //directionRight.Normalize();
            //var positions = new Vector3[6];

            //var originClip = ToClipSpace(originSS.ToVector2(), viewport);
            //var rightClip = ToClipSpace(originSS.ToVector2() + (directionRight * GIZMO_SIZE_IN_PIXELS), viewport);
            //positions[0] = originClip;
            //positions[1] = rightClip;
            //var targetUpSS = viewport.Project(originWS + Vector3.UnitY, m_camera.Projection, m_camera.View, Matrix.Identity);

            //var directionUp = (targetUpSS - originSS).ToVector2();
            //directionUp.Normalize();
            //var upClip = ToClipSpace(originSS.ToVector2() + (directionUp * GIZMO_SIZE_IN_PIXELS), viewport);
            //positions[2] = originClip;
            //positions[3] = upClip;


            //var targetForwardSS = viewport.Project(originWS + Vector3.UnitZ, m_camera.Projection, m_camera.View, Matrix.Identity);
            //var directionForward = (targetForwardSS - originSS).ToVector2();
            //directionForward.Normalize();

            //var forwardClip = ToClipSpace(originSS.ToVector2() + (directionForward * GIZMO_SIZE_IN_PIXELS), viewport);
            //positions[4] = originClip;
            //positions[5] = forwardClip;

            //m_axisGizmoTranslationRenderer.SetLinePositions(positions);
            //for (int i = 0; i < 3; i++)
            //{
            //    var rightInverseNormalized = (i==0 ? viewInverse.Right : (i ==1?viewInverse.Up:viewInverse.Forward));
            //    rightInverseNormalized.Normalize();
            //    var rightViewInverse = Vector3.TransformNormal(rightInverseNormalized, gizmoWorldInverse);

            //    //var rightViewInverse = Vector4.Transform(new Vector4(rightInverseNormalized, viewInverse.M14), gizmoWorldInverse);
            //    //rightViewInverse = Vector4.Transform(rightViewInverse, gizmoWorldInverse);

            //    float rightLength = GetSegmentLengthClipSpace(Vector3.Zero, rightViewInverse);
            //    float screenFactor = GIZMO_SIZE_CLIP_SPACE / rightLength;

            //    var axis = unaryDirections[i];
            //    Vector3 p1 = GetPositionInClipSpace(axis * 0.1f * screenFactor);
            //    Vector3 p2 = GetPositionInClipSpace(axis * screenFactor);

            //    positions[i * 2] = p1;
            //    positions[i * 2 + 1] = p2;
            //}
            //m_axisGizmoTranslationRenderer.SetLinePositions(positions);

            //var gizmoPositionViewSpace = Vector3.Transform(m_selectionState.m_gizmoPosition, Matrix.Invert(m_camera.View));
            //https://es.wikipedia.org/wiki/Tangente_(trigonometr%C3%ADa)
            //m_selectionState.m_screenScaleFactor = Math.Abs( gizmoPositionViewSpace.Z) * (float)Math.Tan(m_camera.FieldOfView*0.5f) * (GIZMO_SCREEN_SCALE );
            //m_selectionState.m_screenScaleFactor = distance * (float)Math.Tan(1) * (GIZMO_SCREEN_SCALE / m_graphicsDevice.Viewport.Height);
            //m_selectionState.m_screenScaleFactor = distance * GIZMO_SCREEN_SCALE;
            //m_selectionState.m_screenScaleFactor = 1.0f;
        }

        private Vector3 GetPositionInClipSpace(Vector3 position)
        {
            var inClip = Vector3.Transform(position, m_selectionState.m_gizmoWorld * m_camera.ViewProjection);
            return inClip;
        }

        public void Render(RenderContext renderContext)
        {
            RenderCameraGizmo(renderContext);
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
                    AxisGizmoRotationRenderer.SetWorld(m_selectionState.m_gizmoWorld);
                    AxisGizmoRotationRenderer.Render(renderContext);
                    break;
                case GizmoType.Scale:
                    m_axisGizmoScaleRenderer.SetWorld(m_selectionState.m_gizmoWorld);
                    m_axisGizmoScaleRenderer.Render(renderContext);
                    break;
                default:
                    break;
            }
        }

        private void RenderCameraGizmo(RenderContext renderContext)
        {
            var savedViewport = renderContext.Viewport;
            var sceneCamera = renderContext.Camera;

            var viewport = new Viewport(renderContext.GameSurfaceTarget.Width - CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS, 0, CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS, CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS);
            renderContext.Viewport = viewport;
            renderContext.GraphicsDevice.Viewport = viewport;
            m_sceneGizmoCamera.AspectRatio = viewport.AspectRatio;
            m_sceneGizmoCamera.FieldOfView = m_camera.FieldOfView;

            var cameraOrientation = Matrix.CreateFromQuaternion(Quaternion.CreateFromMatrix(sceneCamera.View));
            m_sceneGizmoCamera.View = cameraOrientation * Matrix.CreateLookAt(new Vector3(0, 0, m_gizmoConfig.GIZMO_LENGTH + 3.0f), Vector3.Zero, Vector3.Up);
            renderContext.Camera = m_sceneGizmoCamera;
            
            m_axisGizmoTranslationRenderer.SetColors(m_axisColors);
            m_axisGizmoTranslationRenderer.SetWorld(Matrix.Identity);
            m_axisGizmoTranslationRenderer.Render(renderContext);

            m_axisGizmoTranslationRenderer.SetColors(m_activeAxisColors);
            renderContext.Camera = sceneCamera;
            renderContext.Viewport = savedViewport;
            renderContext.GraphicsDevice.Viewport = savedViewport;
        }

        public void SetTransformableAsSelected(IEditorObject editorObject)
        {
            m_selectionState.m_gizmoPosition = editorObject.WorldMatrix.Translation;

            m_objectSelector.Select(editorObject);
            Update();
        }

        public void SelectObjects(Vector2 mousePosition)
        {
            var selectionRay = ConvertMousePositionToRay(mousePosition);

            m_objectSelector.Select(selectionRay);
            if (m_objectSelector.SelectedObjects.Count > 0)
            {

            }
        }

        public ITransformableObject GetSelectedObject()
        {
            if (m_objectSelector.SelectedObjects.Count == 0) return null;

            return m_objectSelector.SelectedObjects[0];
        }
    }
}
