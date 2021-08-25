
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Interop;
using BrunoWindows;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using BrunoApi.Net.Game;
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Gizmos;
using System;

namespace BrunoFramework.Editor.Game.Interaction
{
    public class GizmosHandlerBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        private IGizmoService m_gizmoService;
        private bool m_gizmoSelected;
        private Camera m_currentCamera;

        private Vector3 m_savedPosition, m_savedScale;
        private Quaternion m_savedRotation;
        private Matrix m_savedWorldMatrix;

        public static readonly DependencyProperty CameraProperty = DependencyProperty.Register(
           "Camera",
           typeof(Camera),
           typeof(GizmosHandlerBehavior),
           new FrameworkPropertyMetadata(null));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraProperty); }
            set { SetValue(CameraProperty, value); }
        }

        public static readonly DependencyProperty GizmoServiceProperty = DependencyProperty.Register(
           "GizmoService",
           typeof(GizmoService),
           typeof(GizmosHandlerBehavior),
           new PropertyMetadata(null, OnGizmoServiceChanged));

        public GizmoService GizmoService
        {
            get { return (GizmoService)GetValue(GizmoServiceProperty); }
            set { SetValue(GizmoServiceProperty, value); }
        }

        public static readonly DependencyProperty SceneProperty = DependencyProperty.Register(
           "Scene",
           typeof(Scene),
           typeof(GizmosHandlerBehavior),
           new FrameworkPropertyMetadata(null));

        public Scene Scene
        {
            get { return (Scene)GetValue(SceneProperty); }
            set { SetValue(SceneProperty, value); }
        }

        private static void OnGizmoServiceChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var target = (GizmosHandlerBehavior)dependencyObject;

            var oldValue = (GizmoService)eventArgs.OldValue;
            var newValue = (GizmoService)eventArgs.NewValue;

            if (oldValue != null)
            {
                oldValue.OnTranslationChanged -= target.OnGizmoTranslationChanged;
                oldValue.OnRotateChanged -= target.OnGizmoRotateChanged;
                oldValue.OnScaleChanged -= target.OnGizmoScaleChanged;
            }
            if (newValue == null) return;

            newValue.OnTranslationChanged += target.OnGizmoTranslationChanged;
            newValue.OnRotateChanged += target.OnGizmoRotateChanged;
            newValue.OnScaleChanged += target.OnGizmoScaleChanged;
        }

        private void OnGizmoTranslationChanged(ITransformableObject transformableObject, Vector3 delta)
        {
            var localWorld = Matrix.CreateScale(transformableObject.LocalScale) *
                Matrix.CreateFromQuaternion(transformableObject.LocalRotation) *
                Matrix.CreateTranslation(transformableObject.LocalPosition);

            var toLocal = Matrix.Invert(transformableObject.WorldMatrix) * localWorld;
            var localPosition = Vector3.TransformNormal(delta, toLocal);

            transformableObject.LocalPosition += localPosition;
        }

        private void OnGizmoScaleChanged(ITransformableObject transformableObject, Vector3 delta, bool isUniformScale)
        {
            delta *= 0.1f;
            if (isUniformScale)
            {
                float uniform = 1.0f + (delta.X + delta.Y + delta.Z) / 3.0f;
                //Console.WriteLine("Uniform scale delta = " + uniform + " ; " + m_gizmoService.CurrentGizmoAxis.ToString());
                transformableObject.LocalScale *= uniform;
                return;
            }
            //Console.WriteLine("NU scale delta = " + delta + " ; " + m_gizmoService.CurrentGizmoAxis.ToString());
            transformableObject.LocalScale += delta;
        }

        private void OnGizmoRotateChanged(ITransformableObject transformableObject, Quaternion delta)
        {
            var localRotation = transformableObject.LocalRotation * delta;
            localRotation.Normalize();

            transformableObject.LocalRotation = localRotation;
        }

        protected override void OnAttached()
        {
            base.OnAttached();

            if (WindowsPlatform.InDesignMode)
                return;
            
            AssociatedObject.HwndLButtonDown += OnHwndLButtonDown;
            AssociatedObject.HwndMouseMove += OnHwndMouseMoveSelector;
            AssociatedObject.HwndLButtonDown += OnHwndMouseDownSelector;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndLButtonDown -= OnHwndLButtonDown;
            AssociatedObject.HwndMouseMove -= OnHwndMouseMoveSelector;
            AssociatedObject.HwndLButtonDown -= OnHwndMouseDownSelector;
            base.OnDetaching();
        }

        private void OnHwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            m_gizmoService = GizmoService;
            if (m_gizmoService == null)
                return;

            m_currentCamera = Camera;
            if (m_currentCamera == null) return;

            if (IsKeyAltPressed() || !GizmoService.IsActive)
                return;

            var mousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));
            m_gizmoSelected = m_gizmoService.Begin(mousePosition);

            if (!m_gizmoSelected)
            {
                m_currentCamera = null;
                m_gizmoService = null;
                return;
            }

            var selectedObject = m_gizmoService.GetSelectedObject();
            m_savedPosition = selectedObject.LocalPosition;
            m_savedRotation = selectedObject.LocalRotation;
            m_savedScale = selectedObject.LocalScale;
            m_savedWorldMatrix = selectedObject.WorldMatrix;

            AssociatedObject.LostMouseCapture += OnLostMouseCapture;
            AssociatedObject.HwndMouseMove += OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp += OnHwndLButtonUp;
            AssociatedObject.PreviewKeyDown += OnKeyDown;

            AssociatedObject.CaptureMouse();

            if (!AssociatedObject.IsKeyboardFocused)
            {
                AssociatedObject.Focus();
            }
        }

        private void OnHwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            var mousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));
            GizmoService.UpdateGizmo(mousePosition);
        }

        private void OnHwndLButtonUp(object sender, HwndMouseEventArgs e)
        {
            EndHandler(true);
        }

        private void OnHwndMouseMoveSelector(object sender, HwndMouseEventArgs eventArgs)
        {
            m_gizmoService = GizmoService;
            if (m_gizmoSelected || m_gizmoService == null) return;

            if (IsKeyAltPressed() || !m_gizmoService.IsActive) return;

            m_gizmoService.SetGizmoAxisOverMousePosition(ConvertToVector2(eventArgs.GetPosition(AssociatedObject)));
        }

        private void OnHwndMouseDownSelector(object sender, HwndMouseEventArgs eventArgs)
        {
            m_gizmoService = GizmoService;
            if (IsKeyAltPressed() || m_gizmoSelected || m_gizmoService == null) return;

            m_gizmoService.SelectObjects(ConvertToVector2(eventArgs.GetPosition(AssociatedObject)));
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                EndHandler(false);
                eventArgs.Handled = true;
            }
        }

        private void OnLostMouseCapture(object sender, MouseEventArgs eventArgs)
        {
            EndHandler(false);
            eventArgs.Handled = true;
        }

        private void EndHandler(bool commit)
        {
            m_gizmoService.End();

            AssociatedObject.LostMouseCapture -= OnLostMouseCapture;
            AssociatedObject.HwndMouseMove -= OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp -= OnHwndLButtonUp;
            AssociatedObject.PreviewKeyDown -= OnKeyDown;

            AssociatedObject.ReleaseMouseCapture();

            if (!commit)
            {
                var selectedObject = m_gizmoService.GetSelectedObject();
                selectedObject.LocalPosition = m_savedPosition;
                selectedObject.LocalRotation = m_savedRotation;
                selectedObject.LocalScale = m_savedScale;
                selectedObject.WorldMatrix = m_savedWorldMatrix;
            }
            
            m_gizmoSelected = false;
            m_gizmoService = null;
            m_currentCamera = null;
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }

        private bool IsKeyAltPressed()
        {
            return (Keyboard.Modifiers & ModifierKeys.Alt) == ModifierKeys.Alt;
        }
    }
}
