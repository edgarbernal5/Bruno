
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Interaction
{
    public class CameraHandToolHwndHostBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        struct CameraState
        {
            public Matrix m_view;
            public Vector3 m_target;
            public Vector3 m_position;
        }
        private CameraState m_cameraState;
        private Camera m_currentCamera;

        private Vector2 _previousMousePosition;

        public static readonly DependencyProperty CameraNodeProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraHandToolHwndHostBehavior),
            new FrameworkPropertyMetadata(null));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraNodeProperty); }
            set { SetValue(CameraNodeProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.HwndMButtonDown += OnHwndMButtonDown;
            AssociatedObject.HwndMButtonUp += OnHwndMButtonUp;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndMButtonDown -= OnHwndMButtonDown;
            AssociatedObject.HwndMButtonUp -= OnHwndMButtonUp;
            base.OnDetaching();
        }

        private void OnHwndMButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            if (Mouse.Captured != null && !Equals(Mouse.Captured, AssociatedObject))
            {
                return;
            }

            var cameraNode = Camera;
            if (cameraNode == null)
            {
                return;
            }
            AssociatedObject.CaptureMouse();

            m_currentCamera = cameraNode;
            m_cameraState.m_view = m_currentCamera.View;
            m_cameraState.m_target = m_currentCamera.Target;
            m_cameraState.m_position = m_currentCamera.Position;
            _previousMousePosition = ConvertToVector2(eventArgs.ScreenPosition);

            AssociatedObject.HwndMouseMove += OnHwndMouseMove;
            AssociatedObject.PreviewKeyDown += OnKeyDown;

            if (!AssociatedObject.IsKeyboardFocused)
            {
                AssociatedObject.Focus();
            }
        }

        private void OnHwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            var mousePosition = ConvertToVector2(eventArgs.ScreenPosition);
            var mouseVelocity = GetMouseVelocity(mousePosition);

            var cameraPosition = m_currentCamera.Position;
            var cameraTarget = m_currentCamera.Target;

            var right = Vector3.Cross(cameraTarget - cameraPosition, m_currentCamera.Up);
            right.Normalize();

            var deltaMovement = right * mouseVelocity.X * 0.01f - m_currentCamera.Up * mouseVelocity.Y * 0.01f;
            cameraPosition += deltaMovement;
            cameraTarget += deltaMovement;

            m_currentCamera.Position = cameraPosition;
            m_currentCamera.Target = cameraTarget;
            m_currentCamera.View = Matrix.CreateLookAt(m_currentCamera.Position, m_currentCamera.Target, m_currentCamera.Up);

            _previousMousePosition = ConvertToVector2(eventArgs.ScreenPosition);
        }

        private void OnHwndMButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndOrbit(true);
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                EndOrbit(false);
                eventArgs.Handled = true;
            }
        }

        private void EndOrbit(bool commit)
        {
            AssociatedObject.HwndMouseMove -= OnHwndMouseMove;
            AssociatedObject.PreviewKeyDown -= OnKeyDown;

            AssociatedObject.ReleaseMouseCapture();
            if (!commit)
            {
                m_currentCamera.View = m_cameraState.m_view;
                m_currentCamera.Target = m_cameraState.m_target;
                m_currentCamera.Position = m_cameraState.m_position;
            }

            m_currentCamera = null;
        }

        private Vector2 GetMouseVelocity(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }
    }
}
