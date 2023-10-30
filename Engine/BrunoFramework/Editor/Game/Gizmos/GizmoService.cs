
using BrunoFramework.Editor.Timing;
using BrunoFramework.Graphics;
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Editor;
using System;
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Maths;
using BrunoApi.Net.Graphics.Core;
using Bruno.Logging;

namespace BrunoFramework.Editor.Game.Gizmos
{
    public class GizmoService : IGizmoService
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        public const float GIZMO_SCREEN_SCALE = 0.075f;
        public const float GIZMO_SIZE_CLIP_SPACE = 2.3f;
        public const float CAMERA_GIZMO_SCREEN_SIZE_IN_PIXELS = 60.0f;

        private const float MULTI_AXIS_THICKNESS = 0.05f;
        private const float SINGLE_AXIS_THICKNESS = 0.35f;
        private GizmoConfig m_gizmoConfig;

        private Vector3 m_translationScaleSnapDelta;

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
                var min = new Vector3(m_gizmoConfig.LINE_OFFSET, -SINGLE_AXIS_THICKNESS * 0.5f, -SINGLE_AXIS_THICKNESS * 0.5f);
                var max = new Vector3(m_gizmoConfig.GIZMO_LENGTH, SINGLE_AXIS_THICKNESS * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox YAxisBox
        {
            get
            {
                var min = new Vector3(-SINGLE_AXIS_THICKNESS * 0.5f, m_gizmoConfig.LINE_OFFSET, -SINGLE_AXIS_THICKNESS * 0.5f);
                var max = new Vector3(SINGLE_AXIS_THICKNESS * 0.5f, m_gizmoConfig.GIZMO_LENGTH, SINGLE_AXIS_THICKNESS * 0.5f);

                return new BoundingBox((max + min) * 0.5f,
                                       (max - min) * 0.5f);
            }
        }

        private BoundingBox ZAxisBox
        {
            get
            {
                var min = new Vector3(-SINGLE_AXIS_THICKNESS * 0.5f, -SINGLE_AXIS_THICKNESS * 0.5f, m_gizmoConfig.LINE_OFFSET);
                var max = new Vector3(SINGLE_AXIS_THICKNESS * 0.5f, SINGLE_AXIS_THICKNESS * 0.5f, m_gizmoConfig.GIZMO_LENGTH);

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

        public bool SnapEnabled { get => m_snapEnabled; set => m_snapEnabled = value; }
        private bool m_snapEnabled;

        public bool PrecisionModeEnabled
        {
            get => m_precisionModeEnabled;
            set => m_precisionModeEnabled = value;
        }
        private bool m_precisionModeEnabled;

        public TransformSpace TransformSpace
        {
            get => m_transformSpace;
            set => m_transformSpace = value;
        }

        public GizmoSnappingConfig SnappingConfig => m_gizmoSnappingConfig;
        private GizmoSnappingConfig m_gizmoSnappingConfig;

        private TransformSpace m_transformSpace = TransformSpace.World;

        private class SelectionState
        {
            public Vector3 m_gizmoPosition;

            public float m_screenScaleFactor, m_invScreenScaleFactor;
            public Matrix m_screenScaleMatrix;

            public Matrix m_gizmoObjectOrientedWorld;
            public Matrix m_gizmoAxisAlignedWorld;

            public Matrix m_rotationMatrix;

            public Vector3 m_intersectionPosition, m_prevIntersectionPosition;
            public Vector2 m_prevMousePosition;

            public Plane m_currentGizmoPlane;
            public Matrix m_objectRotation;
        }
        private SelectionState m_selectionState;

        public event Action<ITransformableObject, Vector3> TranslationChanged;
        public event Action<ITransformableObject, Vector3, bool> ScaleChanged;
        public event Action<ITransformableObject, Quaternion> RotateChanged;

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

            m_gizmoSnappingConfig = new GizmoSnappingConfig();

            m_selectionState = new SelectionState();
            m_selectionState.m_rotationMatrix = Matrix.Identity;
            m_selectionState.m_gizmoObjectOrientedWorld = Matrix.Identity;
            m_selectionState.m_gizmoAxisAlignedWorld = Matrix.Identity;
            m_selectionState.m_screenScaleMatrix = Matrix.Identity;
        }

        private GizmoAxis GetAxis(Vector2 mousePosition)
        {
            var closestIntersection = float.MaxValue;
            var selectedAxis = GizmoAxis.None;

            var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoObjectOrientedWorld);

            var ray = ConvertMousePositionToRay(mousePosition);
            ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
            ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

            if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
            {
                float? intersection = XAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestIntersection)
                    {
                        selectedAxis = GizmoAxis.X;
                        closestIntersection = intersection.Value;
                    }
                }
                intersection = YAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestIntersection)
                    {
                        selectedAxis = GizmoAxis.Y;
                        closestIntersection = intersection.Value;
                    }
                }

                intersection = ZAxisBox.Intersects(ray);
                if (intersection.HasValue)
                {
                    if (intersection.Value < closestIntersection)
                    {
                        selectedAxis = GizmoAxis.Z;
                        closestIntersection = intersection.Value;
                    }
                }

                if (m_currentGizmoType == GizmoType.Translation)
                {
                    if (closestIntersection >= float.MaxValue)
                        closestIntersection = float.MinValue;

                    intersection = XYBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value > closestIntersection)
                        {
                            selectedAxis = GizmoAxis.XY;
                            closestIntersection = intersection.Value;
                        }
                    }

                    intersection = XZAxisBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value > closestIntersection)
                        {
                            selectedAxis = GizmoAxis.XZ;
                            closestIntersection = intersection.Value;
                        }
                    }
                    intersection = YZBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value > closestIntersection)
                        {
                            selectedAxis = GizmoAxis.YZ;
                            closestIntersection = intersection.Value;
                        }
                    }
                }
                else if (m_currentGizmoType == GizmoType.Scale)
                {
                    intersection = XYZBox.Intersects(ray);
                    if (intersection.HasValue)
                    {
                        if (intersection.Value < closestIntersection)
                        {
                            selectedAxis = GizmoAxis.XYZ;
                            closestIntersection = intersection.Value;
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
                            if (intersection.Value < closestIntersection)
                            {
                                currentPointOnPlane = positionOnPlane;
                                closestIntersection = intersection.Value;

                                selectedAxis = (GizmoAxis)(i + 1);
                            }
                        }
                    }
                }

                if (selectedAxis != GizmoAxis.None)
                {
                    if (currentPointOnPlane.Length() < m_gizmoConfig.GIZMO_LENGTH * 0.8f)
                    {
                        selectedAxis = GizmoAxis.XYZ;
                    }
                }
            }

            if (closestIntersection >= float.MaxValue || closestIntersection <= float.MinValue)
                selectedAxis = GizmoAxis.None;

            return selectedAxis;
        }

        public bool BeginDrag(Vector2 mousePosition)
        {
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return false;

            var selectedAxis = GetAxis(mousePosition);
            m_currentGizmoAxis = selectedAxis;
            m_currentDelta = Vector3.Zero;

            if (m_currentGizmoAxis == GizmoAxis.None)
                return false;

            if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
                SetGizmoHandlePlaneFor(selectedAxis, mousePosition);

            if (GetAxisIntersectionPoint(mousePosition, out Vector3 intersectionPoint))
            {
                m_selectionState.m_prevMousePosition = mousePosition;
                m_selectionState.m_prevIntersectionPosition = intersectionPoint;
            }

            return true;
        }

        private bool GetAxisIntersectionPoint(Vector2 mousePosition, out Vector3 intersectionPoint)
        {
            intersectionPoint = Vector3.Zero;
            if (m_currentGizmoType == GizmoType.None || !m_isActive)
                return false;

            if (m_currentGizmoType == GizmoType.Translation || m_currentGizmoType == GizmoType.Scale)
            {
                var gizmoWorldInverse = Matrix.Transpose(m_selectionState.m_rotationMatrix);

                var ray = ConvertMousePositionToRay(mousePosition);
                ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
                ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

                SetGizmoHandlePlaneFor(m_currentGizmoAxis, ray);

                var intersection = ray.Intersects(m_selectionState.m_currentGizmoPlane);
                if (intersection.HasValue)
                {
                    intersectionPoint = ray.Position + (ray.Direction * intersection.Value);
                    return true;
                }
            }
            else
            {
                if (m_currentGizmoAxis == GizmoAxis.XYZ)
                {
                    return true;
                }

                var planeNormals = new Vector3[] { m_selectionState.m_rotationMatrix.Right, m_selectionState.m_rotationMatrix.Up, m_selectionState.m_rotationMatrix.Forward };
                
                int planeIndex = ((int)m_currentGizmoAxis) - 1;
                var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoAxisAlignedWorld);

                var ray = ConvertMousePositionToRay(mousePosition);
                ray.Direction = Vector3.TransformNormal(ray.Direction, gizmoWorldInverse);
                ray.Position = Vector3.Transform(ray.Position, gizmoWorldInverse);

                var plane = new Plane(planeNormals[planeIndex], 0);

                var intersection = ray.Intersects(plane);

                if (intersection.HasValue)
                {
                    intersectionPoint = Vector3.Normalize(ray.Position + (ray.Direction * intersection.Value));
                    return true;
                }

            }

            return false;
        }

        private void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, Ray ray)
        {
            var toLocal = Matrix.Transpose(m_selectionState.m_rotationMatrix);

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

                        var newD = -Vector3.Dot(perpendicularRayVector, gizmoPositionInLocal);

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

            //Logger.Debug(string.Format("selected plane normal = {0}; Axis = {1}", plane.Normal, selectedAxis.ToString()));
            m_selectionState.m_currentGizmoPlane = plane;
        }

        private void SetGizmoHandlePlaneFor(GizmoAxis selectedAxis, Vector2 mousePosition)
        {
            var ray = ConvertMousePositionToRay(mousePosition);
            var toLocal = Matrix.Invert(m_selectionState.m_rotationMatrix);
            ray.Position = Vector3.Transform(ray.Position, toLocal);
            ray.Direction = Vector3.TransformNormal(ray.Direction, toLocal);

            SetGizmoHandlePlaneFor(selectedAxis, ray);
        }

        public void Drag(Vector2 mousePosition)
        {
            switch (m_currentGizmoType)
            {
                case GizmoType.Translation:
                    {
                        var translationDelta = GetDeltaMovement(mousePosition);
                        translationDelta = ApplySnapAndPrecisionMode(translationDelta);

                        translationDelta = Vector3.Transform(translationDelta, m_selectionState.m_rotationMatrix);

                        //SetGizmoPosition(translationDelta);

                        if (translationDelta != Vector3.Zero)
                        {
                            if (TranslationChanged != null)
                            {
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    TranslationChanged(m_objectSelector.SelectedObjects[i], translationDelta);
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
                            if (RotateChanged != null)
                            {
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    RotateChanged(m_objectSelector.SelectedObjects[i], rotationDelta);
                                }
                            }
                        }
                    }
                    break;
                case GizmoType.Scale:
                    {
                        var scaleDelta = GetDeltaMovement(mousePosition);
                        scaleDelta = ApplySnapAndPrecisionMode(scaleDelta);

                        if (scaleDelta != Vector3.Zero)
                        {
                            if (m_currentGizmoAxis != GizmoAxis.XYZ)
                            {
                                m_axisGizmoScaleRenderer.UpdateGizmoLength(m_currentGizmoAxis, scaleDelta);
                            }
                            if (ScaleChanged != null)
                            {
                                for (int i = 0; i < m_objectSelector.SelectedObjects.Count; i++)
                                {
                                    ScaleChanged(m_objectSelector.SelectedObjects[i], scaleDelta, m_currentGizmoAxis == GizmoAxis.XYZ);
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

        private Vector3 ApplySnapAndPrecisionMode(Vector3 delta)
        {
            if (SnapEnabled)
            {
                var snapValue = m_currentGizmoType == GizmoType.Scale ?
                    m_gizmoSnappingConfig.Scale : 
                    m_gizmoSnappingConfig.Translation;
                
                if (PrecisionModeEnabled)
                {
                    delta *= m_gizmoSnappingConfig.PrecisionScale;
                    snapValue *= m_gizmoSnappingConfig.PrecisionScale;
                }
                m_translationScaleSnapDelta += delta;

                delta = new Vector3((int)(m_translationScaleSnapDelta.X / snapValue) * snapValue,
                    (int)(m_translationScaleSnapDelta.Y / snapValue) * snapValue,
                    (int)(m_translationScaleSnapDelta.Z / snapValue) * snapValue);

                m_translationScaleSnapDelta -= delta;
            }
            else if (PrecisionModeEnabled)
            {
                delta *= m_gizmoSnappingConfig.PrecisionScale;
            }

            return delta;
        }

        public void OnMouseMove(Vector2 mousePosition)
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
            var rotationDelta = Quaternion.Identity;
            if (m_currentGizmoAxis == GizmoAxis.XYZ)
            {
                var gizmoScreenPosition = GetScreenPosition(m_selectionState.m_gizmoPosition);
                var gizmoScreenPosition2 = GetScreenPosition(m_selectionState.m_gizmoPosition + m_camera.View.Right * m_gizmoConfig.GIZMO_LENGTH);
                var length = 3.0f * (gizmoScreenPosition2 - gizmoScreenPosition).Length() / MathHelper.TwoPi;
                var deltaAngles = new Vector2(1.0f / length);

                var mouseVelocity = new Vector2(mousePosition.X - m_selectionState.m_prevMousePosition.X, mousePosition.Y - m_selectionState.m_prevMousePosition.Y);

                var angles = mouseVelocity * deltaAngles;

                rotationDelta = Quaternion.CreateFromYawPitchRoll(angles.X, 0, 0) * Quaternion.CreateFromYawPitchRoll(0, angles.Y, 0);
            }
            else
            {
                var planeNormals = new Vector3[] { m_selectionState.m_rotationMatrix.Right, m_selectionState.m_rotationMatrix.Up, m_selectionState.m_rotationMatrix.Forward };
                int planeIndex = ((int)m_currentGizmoAxis) - 1;

                var gizmoWorldInverse = Matrix.Invert(m_selectionState.m_gizmoAxisAlignedWorld);

                var ray = ConvertMousePositionToRay(mousePosition);
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

                    var sign = Vector3.Dot(perpendicularVector, planeNormals[planeIndex]) < 0.0f ? -1.0f : 1.0f;

                    //Logger.Debug("positionOnPlane = {0}, sign = {1}, angle = {2}, ray.position = {3}", positionOnPlane, sign, angle, ray.Position);
                    delta = sign * angle;
                    //delta *= (float)m_gameTimer.ElapsedTime.TotalSeconds;
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
            m_selectionState.m_prevIntersectionPosition = m_selectionState.m_intersectionPosition;
            return rotationDelta;
        }

        private Vector3 GetDeltaMovement(Vector2 mousePosition)
        {
            Vector3 delta = Vector3.Zero;
            if (GetAxisIntersectionPoint(mousePosition, out Vector3 intersectionPoint))
            {
                m_selectionState.m_intersectionPosition = intersectionPoint;
                m_currentDelta = intersectionPoint - m_selectionState.m_prevIntersectionPosition;

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
            var selectionPosition = Vector3.Zero;

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

        public void EndDrag()
        {
            OnMouseMove(m_selectionState.m_prevMousePosition);

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

            nearPoint = m_camera.Viewport.Unproject(nearPoint,
                                                m_camera.Projection,
                                                m_camera.View,
                                                Matrix.Identity);

            farPoint = m_camera.Viewport.Unproject(farPoint,
                                            m_camera.Projection,
                                            m_camera.View,
                                            Matrix.Identity);

            Vector3 direction = farPoint - nearPoint;
            direction.Normalize();

            return new Ray(nearPoint, direction);
        }

        private Vector2 GetScreenPosition(Vector3 worldPosition)
        {
            var point = m_camera.Viewport.Project(worldPosition,
                                                m_camera.Projection,
                                                m_camera.View,
                                                Matrix.Identity);

            return new Vector2(point.X, point.Y);
        }

        private void Update()
        {
            if (m_camera == null) return;

            var gizmoPositionViewSpace = Vector3.Transform(m_selectionState.m_gizmoPosition, m_camera.View);
            m_selectionState.m_screenScaleFactor = Math.Abs(gizmoPositionViewSpace.Z) * GIZMO_SCREEN_SCALE;
            
            //m_selectionState.m_screenScaleFactor = gizmoPositionViewSpace.Length() * GIZMO_SCREEN_SCALE;
            //float scale = (float)Math.Tan(m_camera.FieldOfView * 0.5f) * (m_camera.Position - m_selectionState.m_gizmoPosition).Length() * 2.0f;
            //m_selectionState.m_screenScaleFactor = scale * GIZMO_SCREEN_SCALE;

            if (m_selectionState.m_screenScaleFactor < 0.0001f)
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f;
            }
            else
            {
                m_selectionState.m_invScreenScaleFactor = 1.0f / m_selectionState.m_screenScaleFactor;
            }

            m_selectionState.m_screenScaleMatrix = Matrix.CreateScale(new Vector3(m_selectionState.m_screenScaleFactor));

            if (m_objectSelector.SelectedObjects.Count == 0)
            {
                return;
            }
            m_selectionState.m_rotationMatrix = Matrix.Identity;

            if (m_currentGizmoType == GizmoType.Translation ||
                m_currentGizmoType == GizmoType.Rotation)
            {
                var world = Matrix.Identity;

                if (m_transformSpace == TransformSpace.Local)
                {
                    world = m_selectionState.m_objectRotation;
                }

                m_selectionState.m_gizmoObjectOrientedWorld = m_selectionState.m_screenScaleMatrix *
                                                        Matrix.CreateWorld(m_selectionState.m_gizmoPosition, world.Forward, world.Up);

                m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_screenScaleMatrix *
                                                        Matrix.CreateWorld(m_selectionState.m_gizmoPosition, Vector3.Forward, Vector3.Up);

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

                m_selectionState.m_gizmoObjectOrientedWorld = m_selectionState.m_screenScaleMatrix *
                                                        Matrix.CreateWorld(m_selectionState.m_gizmoPosition, localForward, localUp);

                m_selectionState.m_gizmoAxisAlignedWorld = m_selectionState.m_gizmoObjectOrientedWorld;

                m_selectionState.m_rotationMatrix.Forward = localForward;
                m_selectionState.m_rotationMatrix.Up = localUp;
                m_selectionState.m_rotationMatrix.Right = localRight;
            }

            //var gizmoPositionViewSpace = Vector3.Transform(m_selectionState.m_gizmoPosition, Matrix.Invert(m_camera.View));
            //https://es.wikipedia.org/wiki/Tangente_(trigonometr%C3%ADa)
            //m_selectionState.m_screenScaleFactor = Math.Abs( gizmoPositionViewSpace.Z) * (float)Math.Tan(m_camera.FieldOfView*0.5f) * (GIZMO_SCREEN_SCALE );
            //m_selectionState.m_screenScaleFactor = distance * (float)Math.Tan(1) * (GIZMO_SCREEN_SCALE / m_graphicsDevice.Viewport.Height);
            //m_selectionState.m_screenScaleFactor = distance * GIZMO_SCREEN_SCALE;
            //m_selectionState.m_screenScaleFactor = 1.0f;
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
                    m_axisGizmoTranslationRenderer.SetWorld(m_selectionState.m_gizmoObjectOrientedWorld);
                    m_axisGizmoTranslationRenderer.Render(renderContext);
                    break;
                case GizmoType.Rotation:
                    AxisGizmoRotationRenderer.SetWorld(m_selectionState.m_gizmoObjectOrientedWorld);
                    AxisGizmoRotationRenderer.Render(renderContext);
                    break;
                case GizmoType.Scale:
                    m_axisGizmoScaleRenderer.SetWorld(m_selectionState.m_gizmoObjectOrientedWorld);
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
            m_sceneGizmoCamera.Viewport = viewport;
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

        public void SetEditorObjectAsSelected(IEditorObject editorObject)
        {
            m_selectionState.m_gizmoPosition = editorObject.WorldMatrix.Translation;
            m_selectionState.m_objectRotation = Matrix.CreateFromQuaternion(editorObject.LocalRotation);

            m_objectSelector.Select(editorObject);
            Update();
        }

        public void UpdateSelectedObject(IEditorObject editorObject)
        {
            m_selectionState.m_gizmoPosition = editorObject.WorldMatrix.Translation; //TODO: hay que ver si esto eaplica cuando se seleccionen mas de un objeto
            m_selectionState.m_objectRotation = Matrix.CreateFromQuaternion(editorObject.LocalRotation);

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
            if (m_objectSelector.SelectedObjects.Count == 0) return default;

            return m_objectSelector.SelectedObjects[0];
        }
    }
}
