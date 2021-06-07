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
    public class CameraZoomHwndHostBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        public static readonly DependencyProperty CameraProperty = DependencyProperty.Register(
            "Camera",
            typeof(Camera),
            typeof(CameraZoomHwndHostBehavior),
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

            AssociatedObject.HwndMouseWheel += OnMouseWheel;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndMouseWheel -= OnMouseWheel;
            base.OnDetaching();
        }

        private void OnMouseWheel(object sender, HwndMouseEventArgs eventArgs)
        {
            var currentCamera = Camera;
            if (currentCamera == null)
                return;

            float wheelDelta = eventArgs.WheelDelta * 0.001f;

            var viewDirection = Vector3.Normalize(currentCamera.Target - currentCamera.Position);
            var cameraPosition = currentCamera.Position + viewDirection * wheelDelta;

            if (Vector3.Distance(cameraPosition, currentCamera.Target) < 0.5f)
                return;

            currentCamera.Position = cameraPosition;
            //agregar modificadores de wheelDelta (tecla shift o ctrl)
        }
    }
}
