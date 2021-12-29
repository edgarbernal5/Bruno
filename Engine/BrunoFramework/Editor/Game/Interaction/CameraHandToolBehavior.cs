
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Interop;
using BrunoWindows;
using System.Windows;
using System.Windows.Input;
using Microsoft.Xaml.Behaviors;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Editor.Game.Interaction
{
    public class CameraHandToolBehavior : Behavior<GameSurfaceTargetHwndHost>
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

        public static readonly DependencyProperty CameraProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraHandToolBehavior),
            new FrameworkPropertyMetadata(null));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraProperty); }
            set { SetValue(CameraProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();

            if (WindowsPlatform.InDesignMode)
                return;

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

            var deltaMovement = right * mouseVelocity.X * 0.1f - m_currentCamera.Up * mouseVelocity.Y * 0.1f;
            cameraPosition += deltaMovement;
            cameraTarget += deltaMovement;

            m_currentCamera.Position = cameraPosition;
            m_currentCamera.Target = cameraTarget;

            _previousMousePosition = ConvertToVector2(eventArgs.ScreenPosition);
        }

        private void OnHwndMButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndPan(true);
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                EndPan(false);
                eventArgs.Handled = true;
            }
        }

        private void EndPan(bool commit)
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
