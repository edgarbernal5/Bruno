
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using System.Windows.Interop;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Interaction
{
    public class CameraMouseHwndHostBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        private Camera _currentCamera;
        private Matrix _originalViewMatrix;
        private Vector2 _previousMousePosition;
        private Vector2 yawPitch;

        public static readonly DependencyProperty CameraNodeProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraMouseHwndHostBehavior),
            new FrameworkPropertyMetadata(null));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraNodeProperty); }
            set { SetValue(CameraNodeProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.HwndLButtonDown += OnHwndLButtonDown;
            AssociatedObject.HwndMouseWheel += OnHwndMouseWheel;
        }

        private void OnHwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            BeginOrbit(eventArgs);
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndLButtonDown -= OnHwndLButtonDown;
            AssociatedObject.HwndMouseWheel -= OnHwndMouseWheel;
            base.OnDetaching();
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

            _currentCamera = cameraNode;
            _originalViewMatrix = _currentCamera.View;

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

        private void OnHwndLButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            EndOrbit(true);
        }

        private void OnHwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            if (eventArgs.LeftButton == MouseButtonState.Pressed)
            {
                var currentMousePosition = ConvertToVector2(eventArgs.ScreenPosition);
                var localRotation = GetMouseSpeed(currentMousePosition);
                if (localRotation != Vector2.Zero)
                {
                    yawPitch += localRotation * (float)GameUnit.m_gameStepTimer.ElapsedTime.TotalSeconds * 0.5f;
                    _currentCamera.Rotation = Quaternion.CreateFromYawPitchRoll(yawPitch.X, yawPitch.Y, 0);
                }

                _currentCamera.Recalculate();
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
                // Operation canceled, revert camera pose.
                _currentCamera.View = _originalViewMatrix;
            }

            _currentCamera = null;
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

        private void OnHwndMouseWheel(object sender, HwndMouseEventArgs eventArgs)
        {
            var cameraNode = Camera;
            if (cameraNode == null)
                return;

            float wheel = (float)eventArgs.WheelDelta * 0.01f;
            float distance = cameraNode.Distance - wheel;

            cameraNode.Distance = distance;
            cameraNode.Recalculate();
        }

        private Vector2 GetMouseSpeed(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }
    }
}
