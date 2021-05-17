
using EsteroFramework.Editor.Units;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Interop;
using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using TrioApi.Net.Maths;

namespace EsteroFramework.Editor.Game.Interaction
{
    public class CameraMouseBehavior : Behavior<GameSurfaceTarget>
    {
        private Camera _currentCamera;
        private Matrix _originalViewMatrix;
        private Vector2 _previousMousePosition;
        private Vector2 yawPitch;

        public static readonly DependencyProperty CameraNodeProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraMouseBehavior),
            new FrameworkPropertyMetadata(null));

        public Camera Camera
        {
            get { return (Camera)GetValue(CameraNodeProperty); }
            set { SetValue(CameraNodeProperty, value); }
        }

        protected override void OnAttached()
        {
            base.OnAttached();
            AssociatedObject.MouseDown += OnMouseDown;
            AssociatedObject.MouseWheel += OnMouseWheel;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.MouseDown -= OnMouseDown;
            AssociatedObject.MouseWheel -= OnMouseWheel;
            base.OnDetaching();
        }

        private void OnMouseWheel(object sender, MouseWheelEventArgs eventArgs)
        {
            if (eventArgs.Handled)
                return;

            var cameraNode = Camera;
            if (cameraNode == null)
                return;

            float wheel = (float)eventArgs.Delta*0.01f;
            float distance = cameraNode.Distance - wheel;

            cameraNode.Distance = distance;
            cameraNode.Recalculate();

            eventArgs.Handled = true;
        }

        private void OnMouseDown(object sender, MouseButtonEventArgs eventArgs)
        {
            if (!eventArgs.Handled && eventArgs.ChangedButton == MouseButton.Left)
                BeginOrbit(eventArgs);
        }

        private void BeginOrbit(MouseButtonEventArgs eventArgs)
        {
            if (Mouse.Captured != null && !Equals(Mouse.Captured, AssociatedObject))
            {
                // Mouse is already captured by another element.
                return;
            }

            var cameraNode = Camera;
            if (cameraNode == null)
            {
                // No camera found.
                return;
            }
            if (!AssociatedObject.CaptureMouse())
            {
                // Failed to capture mouse.
                return;
            }

            _currentCamera = cameraNode;
            _originalViewMatrix = _currentCamera.View;

            AssociatedObject.PreviewKeyDown += OnKeyDown;
            AssociatedObject.LostMouseCapture += OnLostMouseCapture;
            AssociatedObject.MouseMove += OnMouseMove;
            AssociatedObject.MouseUp += OnMouseUp;

            _previousMousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));

            if (!AssociatedObject.IsKeyboardFocused)
            {
                // Focus element to receive keyboard events.
                AssociatedObject.Focus();
            }

            eventArgs.Handled = true;
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }

        private void EndOrbit(bool commit)
        {
            AssociatedObject.PreviewKeyDown -= OnKeyDown;
            AssociatedObject.LostMouseCapture -= OnLostMouseCapture;
            AssociatedObject.MouseMove -= OnMouseMove;
            AssociatedObject.MouseUp -= OnMouseUp;

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

        private void OnMouseUp(object sender, MouseButtonEventArgs eventArgs)
        {
            if (eventArgs.ChangedButton == MouseButton.Left)
            {
                // End orbiting, commit camera pose.
                EndOrbit(true);
                eventArgs.Handled = true;
            }
        }

        private void OnMouseMove(object sender, MouseEventArgs eventArgs)
        {
            if (eventArgs.LeftButton == MouseButtonState.Pressed)
            {
                var currentMousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));
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

        private Vector2 GetMouseSpeed(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }
    }
}
