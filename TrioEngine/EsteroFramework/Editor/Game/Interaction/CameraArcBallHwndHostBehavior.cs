
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using EsteroWindows;
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
        private bool m_canRotate;

        private Vector2 _previousMousePosition;
        private Vector2 m_currentYawPitch;

        public static readonly DependencyProperty CameraProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraArcBallHwndHostBehavior),
            new PropertyMetadata(null, OnCameraChanged));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraProperty); }
            set { SetValue(CameraProperty, value); }
        }

        private static void OnCameraChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var target = (CameraArcBallHwndHostBehavior)dependencyObject;

            var newValue = (Camera)eventArgs.NewValue;
            if (newValue == null) return;

            target.GetInitialYawPitch(newValue);
        }

        protected override void OnAttached()
        {
            base.OnAttached();

            if (WindowsPlatform.InDesignMode)
                return;

            AssociatedObject.HwndLButtonDown += OnHwndLButtonDown;
            AssociatedObject.PreviewKeyDown += OnKeyDown;
            AssociatedObject.PreviewKeyUp += OnKeyUp;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndLButtonDown -= OnHwndLButtonDown;
            AssociatedObject.PreviewKeyDown -= OnKeyDown;
            AssociatedObject.PreviewKeyUp -= OnKeyUp;
            base.OnDetaching();
        }

        private void OnHwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            BeginOrbit(eventArgs);
        }

        private void BeginOrbit(HwndMouseEventArgs eventArgs)
        {
            //TODO: pasar esta logica al HwndHost. No estoy seguro de esto (? 50% de esto 
            if (Mouse.Captured != null && !Equals(Mouse.Captured, AssociatedObject))
            {
                // Mouse is already captured by another element.
                return;
            }

            if (!AssociatedObject.IsKeyboardFocused)
            {
                AssociatedObject.Focus();
            }

            if (!m_canRotate) return;

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

            _previousMousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));

            m_currentCamera = cameraNode;
            m_cameraState.m_view = m_currentCamera.View;
            m_cameraState.m_target = m_currentCamera.Target;
            m_cameraState.m_position = m_currentCamera.Position;
            m_cameraState.m_up = m_currentCamera.Up;
            m_cameraState.m_yawPitch = m_currentYawPitch;

            AssociatedObject.HwndMouseMove += OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp += OnHwndLButtonUp;

            AssociatedObject.LostMouseCapture += OnLostMouseCapture;
        }

        private void GetInitialYawPitch(Camera camera)
        {
            var yawPitchRoll = Quaternion.EulerAngles(Quaternion.Inverse(Quaternion.CreateFromMatrix(camera.View)));

            m_currentYawPitch.X = yawPitchRoll.X;
            m_currentYawPitch.Y = yawPitchRoll.Y;
        }

        private void OnHwndLButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndOrbit(true);
        }

        private void CalculateViewMatrix(Quaternion rotation)
        {
            float distance = (m_currentCamera.Position - m_currentCamera.Target).Length();
            Vector3 translation = new Vector3(0.0f, 0.0f, distance);
            translation = Vector3.Transform(translation, rotation);

            m_currentCamera.Position = m_currentCamera.Target + translation;

            m_currentCamera.Up = Vector3.Transform(Vector3.Up, rotation);
        }

        private void OnHwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            if (eventArgs.LeftButton == MouseButtonState.Pressed)
            {
                var currentMousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));

                var viewportWidth = (float)AssociatedObject.ActualWidth;
                var viewportHeight = (float)AssociatedObject.ActualHeight;
                
                var deltaAngles = new Vector2(2.0f * MathHelper.PI / viewportWidth, MathHelper.PI / viewportHeight);

                var mouseVelocity = GetMouseVelocity(currentMousePosition);
                var angles = mouseVelocity * deltaAngles;

                m_currentYawPitch += angles;
                Quaternion rotation = Quaternion.CreateFromYawPitchRoll(m_currentYawPitch.X, m_currentYawPitch.Y, 0);

                CalculateViewMatrix(rotation);
                _previousMousePosition = currentMousePosition;
            }
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }

        private void EndOrbit(bool commit)
        {
            AssociatedObject.LostMouseCapture -= OnLostMouseCapture;

            AssociatedObject.HwndMouseMove -= OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp -= OnHwndLButtonUp;

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
            m_canRotate = false;
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                EndOrbit(false);
                eventArgs.Handled = true;
            }
            if (eventArgs.Key == Key.System && eventArgs.SystemKey == Key.LeftAlt)
            {
                m_canRotate = true;
                eventArgs.Handled = true;
            }
        }

        private void OnKeyUp(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.System && eventArgs.SystemKey == Key.LeftAlt)
            {
                m_canRotate = false;
                eventArgs.Handled = true;
            }
        }

        private void OnLostMouseCapture(object sender, MouseEventArgs eventArgs)
        {
            EndOrbit(false);
            eventArgs.Handled = true;
            m_canRotate = false;
        }

        private Vector2 GetMouseVelocity(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }
    }
}
