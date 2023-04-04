
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Interop;
using BrunoWindows;
using System;
using System.Windows;
using System.Windows.Input;
using Microsoft.Xaml.Behaviors;
using BrunoApi.Net.Maths;

namespace BrunoFramework.Editor.Game.Interaction
{
    /*
     * https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
     */
    public class CameraArcBallBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        struct CameraState
        {
            public Matrix m_view;
            public Vector3 m_target;
            public Vector3 m_position;
            public Vector3 m_up;
        }
        private CameraState m_cameraState;
        private Camera m_currentCamera;
        private bool m_canRotate;

        private Vector2 m_previousMousePosition;

        public static readonly DependencyProperty CameraProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraArcBallBehavior),
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

            var camera = Camera;
            if (camera == null)
            {
                return;
            }
            AssociatedObject.CaptureMouse();
            //if (!AssociatedObject.CaptureMouse())
            //{
            //    // Failed to capture mouse.
            //    return;
            //}

            m_previousMousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));

            m_currentCamera = camera;
            m_cameraState.m_view = m_currentCamera.View;
            m_cameraState.m_target = m_currentCamera.Target;
            m_cameraState.m_position = m_currentCamera.Position;
            m_cameraState.m_up = m_currentCamera.Up;

            AssociatedObject.HwndMouseMove += OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp += OnHwndLButtonUp;

            AssociatedObject.LostMouseCapture += OnLostMouseCapture;
        }

        private void OnHwndLButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndOrbit(true);
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

                var directionToTarget = Vector3.Normalize(m_currentCamera.Target - m_currentCamera.Position);

                var upPerpendicular = Vector3.Cross(m_currentCamera.Up, directionToTarget);
                upPerpendicular = Vector3.Cross(directionToTarget, upPerpendicular);

                var rightWorld = Vector3.TransformNormal(Vector3.Right, Matrix.Invert(m_currentCamera.View));
                var rotationMatrixX = Matrix.CreateFromAxisAngle(rightWorld, angles.Y);
                var position = Vector3.Transform(m_currentCamera.Position - m_currentCamera.Target, rotationMatrixX) + m_currentCamera.Target;

                var rotationMatrixY = Matrix.CreateFromAxisAngle(Vector3.Up, angles.X);
                var finalPosition = Vector3.Transform(position - m_currentCamera.Target, rotationMatrixY) + m_currentCamera.Target;

                upPerpendicular = Vector3.TransformNormal(upPerpendicular, rotationMatrixX);
                upPerpendicular = Vector3.TransformNormal(upPerpendicular, rotationMatrixY);
                
                m_currentCamera.Position = finalPosition;
                m_currentCamera.Up = upPerpendicular;

                m_previousMousePosition = currentMousePosition;
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
            }

            m_currentCamera = null;
            m_canRotate = false;
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape && m_currentCamera != null)
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

        private Vector2 GetMouseVelocity(Vector2 currentMousePosition) => m_previousMousePosition - currentMousePosition;
    }
}
