
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Interaction
{
    /*
     * https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
     */
    public class CameraArcBallHwndHostBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        struct CameraState
        {
            public Matrix m_view;
            public Vector3 m_target;
            public Vector3 m_position;
            public Vector3 m_up;
            public Vector2 m_yawPitch;
        }
        private CameraState m_cameraState;
        private Camera m_currentCamera;

        private Vector2 _previousMousePosition;
        private Vector2 m_currentYawPitch;

        public static readonly DependencyProperty CameraNodeProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraArcBallHwndHostBehavior),
            new PropertyMetadata(null, OnCameraChanged));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraNodeProperty); }
            set { SetValue(CameraNodeProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.HwndLButtonDown += OnHwndLButtonDown;
        }

        private void OnHwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            BeginOrbit(eventArgs);
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndLButtonDown -= OnHwndLButtonDown;
            base.OnDetaching();
        }

        private static void OnCameraChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var target = (CameraArcBallHwndHostBehavior)dependencyObject;
            
            var newValue = (Camera)eventArgs.NewValue;
            target.GetInitialYawPitch(newValue);
        }

        private void BeginOrbit(HwndMouseEventArgs eventArgs)
        {
            if (Mouse.Captured != null && !Equals(Mouse.Captured, AssociatedObject))
            {
                // Mouse is already captured by another element.
                return;
            }

            var cameraNode = Camera;
            if (cameraNode == null)
            {
                return;
            }
            AssociatedObject.CaptureMouse();
            //if (!AssociatedObject.CaptureMouse())
            //{
            //    // Failed to capture mouse.
            //    return;
            //}

            m_currentCamera = cameraNode;
            m_cameraState.m_view = m_currentCamera.View;
            m_cameraState.m_target = m_currentCamera.Target;
            m_cameraState.m_position = m_currentCamera.Position;
            m_cameraState.m_up = m_currentCamera.Up;
            m_cameraState.m_yawPitch = m_currentYawPitch;

            AssociatedObject.HwndMouseMove += OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp += OnHwndLButtonUp;
            AssociatedObject.PreviewKeyDown += OnKeyDown;

            //AssociatedObject.LostMouseCapture += OnLostMouseCapture;

            _previousMousePosition = ConvertToVector2(eventArgs.ScreenPosition);

            if (!AssociatedObject.IsKeyboardFocused)
            {
                // Focus element to receive keyboard events.
                AssociatedObject.Focus();
            }

            //eventArgs.Handled = true;
        }

        private void GetInitialYawPitch(Camera camera)
        {
            var viewMatrix = Matrix.CreateLookAt(camera.Position, camera.Target, camera.Up);

            var yawPitchRoll = Quaternion.EulerAngles(Quaternion.Inverse(Quaternion.CreateFromMatrix(viewMatrix)));

            m_currentYawPitch.X = yawPitchRoll.X;
            m_currentYawPitch.Y = yawPitchRoll.Y;
        }

        private void OnHwndLButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndOrbit(true);
        }

        private void RecalculateViewMatrix(Quaternion rotation)
        {
            float distance = (m_currentCamera.Position - m_currentCamera.Target).Length();
            Vector3 translation = new Vector3(0.0f, 0.0f, distance);
            translation = Vector3.Transform(translation, rotation);

            m_currentCamera.Position = m_currentCamera.Target + translation;

            m_currentCamera.Up = Vector3.Transform(Vector3.Up, rotation);

            m_currentCamera.View = Matrix.CreateLookAt(m_currentCamera.Position, m_currentCamera.Target, m_currentCamera.Up);
        }

        private void OnHwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            if (eventArgs.LeftButton == MouseButtonState.Pressed)
            {
                var currentMousePosition = ConvertToVector2(eventArgs.ScreenPosition);

                var viewportWidth = (float)AssociatedObject.ActualWidth;
                var viewportHeight = (float)AssociatedObject.ActualHeight;
                
                var deltaAngles = new Vector2(2.0f * MathHelper.Pi / viewportWidth, MathHelper.Pi / viewportHeight);

                var mouseVelocity = GetMouseVelocity(currentMousePosition);
                var angles = mouseVelocity * deltaAngles;

                m_currentYawPitch += angles;
                Quaternion rotation = Quaternion.CreateFromYawPitchRoll(m_currentYawPitch.X, m_currentYawPitch.Y, 0);

                RecalculateViewMatrix(rotation);
                _previousMousePosition = currentMousePosition;
            }
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }

        private void EndOrbit(bool commit)
        {
            //AssociatedObject.LostMouseCapture -= OnLostMouseCapture;

            AssociatedObject.HwndMouseMove -= OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp -= OnHwndLButtonUp;
            AssociatedObject.PreviewKeyDown -= OnKeyDown;

            AssociatedObject.ReleaseMouseCapture();

            if (!commit)
            {
                m_currentCamera.View = m_cameraState.m_view;
                m_currentCamera.Target = m_cameraState.m_target;
                m_currentCamera.Position = m_cameraState.m_position;
                m_currentCamera.Up = m_cameraState.m_up;
                m_currentYawPitch = m_cameraState.m_yawPitch;
            }

            m_currentCamera = null;
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                // Cancel orbiting, revert camera pose.
                EndOrbit(false);
                eventArgs.Handled = true;
            }
        }

        private void OnLostMouseCapture(object sender, MouseEventArgs eventArgs)
        {
            // Cancel orbiting, revert camera pose.
            EndOrbit(false);
            eventArgs.Handled = true;
        }

        private Vector2 GetMouseVelocity(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }
    }
}
