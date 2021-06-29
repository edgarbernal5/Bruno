
using BrunoFramework.Graphics.Data;
using BrunoFramework.Graphics.Interop;
using BrunoWindows;
using System.Windows;
using System.Windows.Input;
using System.Windows.Interactivity;
using BrunoApi.Net.Game;
using BrunoApi.Net.Maths;
using BrunoFramework.Editor.Game.Gizmos;

namespace BrunoFramework.Editor.Game.Interaction
{
    public class GizmosHandlerBehavior : Behavior<GameSurfaceTargetHwndHost>
    {
        private IGizmoService m_gizmoService;
        private bool m_gizmoSelected;
        private Camera m_currentCamera;

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
            }
            if (newValue == null) return;

            newValue.OnTranslationChanged += target.OnGizmoTranslationChanged;
        }

        private void OnGizmoTranslationChanged(ITransformable gizmoTransformable, Vector3 delta)
        {
            //var scene = Scene;
            //scene.TransformTranslate(gizmoTransformable.Id, delta);
        }

        private void OnGizmoScaleChanged(ITransformable gizmoTransformable, Vector3 delta, bool isUniformScale)
        {
            //var scene = Scene;
            //scene.TransformScale(gizmoTransformable.Id, delta);
        }

        protected override void OnAttached()
        {
            base.OnAttached();

            if (WindowsPlatform.InDesignMode)
                return;
            
            AssociatedObject.HwndLButtonDown += OnHwndLButtonDown;
            AssociatedObject.HwndMouseMove += OnHwndMouseMoveSelector;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.HwndLButtonDown -= OnHwndLButtonDown;
            AssociatedObject.HwndMouseMove -= OnHwndMouseMoveSelector;
            base.OnDetaching();
        }

        private void OnHwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            m_gizmoService = GizmoService;
            if (m_gizmoService == null)
                return;

            m_currentCamera = Camera;
            if (m_currentCamera == null) return;

            if (!GizmoService.IsActive)
                return;

            var mousePosition = ConvertToVector2(eventArgs.GetPosition(AssociatedObject));
            m_gizmoSelected = m_gizmoService.Begin(mousePosition);

            if (!m_gizmoSelected)
            {
                m_currentCamera = null;
                m_gizmoService = null;
                return;
            }

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

            if (!m_gizmoService.IsActive) return;

            m_gizmoService.SetGizmoAxis(ConvertToVector2(eventArgs.GetPosition(AssociatedObject)));
        }

        private void OnKeyDown(object sender, KeyEventArgs eventArgs)
        {
            if (eventArgs.Key == Key.Escape)
            {
                EndHandler(false);
                eventArgs.Handled = true;
            }
        }

        private void EndHandler(bool commit)
        {
            m_gizmoService.End();

            AssociatedObject.HwndMouseMove -= OnHwndMouseMove;
            AssociatedObject.HwndLButtonUp -= OnHwndLButtonUp;
            AssociatedObject.PreviewKeyDown -= OnKeyDown;

            AssociatedObject.ReleaseMouseCapture();

            if (!commit)
            {

            }
            
            m_gizmoSelected = false;
            m_gizmoService = null;
            m_currentCamera = null;
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }
    }
}
