using EsteroFramework.Graphics;
using EsteroFramework.Graphics.Data;
using EsteroFramework.Graphics.Editor;
using System.Windows;
using TrioApi.Net.Graphics.Core;
using TrioApi.Net.Maths;

namespace EsteroGraphicsHost
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private GridMesh m_gridMesh;
        private RenderContext m_renderContext;
        private bool m_mouseDown;
        private Vector2 _previousMousePosition;
        private Vector2 yawPitch;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void GraphicsDeviceHost_OnRender()
        {
            var camera = m_renderContext.Camera;
            camera.AspectRatio = m_renderContext.GraphicsDevice.Viewport.AspectRatio;
            m_renderContext.Viewport = m_renderContext.GraphicsDevice.Viewport;

            GraphicsDeviceService.Service.GraphicsDevice.Clear(Color.Red);

            float time = (float)GraphicsDeviceHost.m_gameStepTimer.TotalTime.TotalSeconds;
           // camera.Rotation = Quaternion.CreateFromYawPitchRoll(time*0.1f, -0.3f, 0);

            //camera.Recalculate();
            m_gridMesh.Render(m_renderContext);
        }

        private void GraphicsDeviceHost_OnLoaded()
        {
            //var device = GraphicsDeviceService.Service.GraphicsDevice;
            //m_gridMesh = new GridMesh(device, 32);
            //m_renderContext = new RenderContext();
            //m_renderContext.GraphicsDevice = device;
            //m_renderContext.Viewport = device.Viewport;

            //var camera = new Camera();
            //camera.FieldOfView = 60.0f * 3.1416f / 180.0f;
            //camera.NearPlane = 0.1f;
            //camera.FarPlane = 1000.0f;
            //camera.AspectRatio = device.Viewport.AspectRatio;

            //camera.Position = new Vector3(70.0f, 70.0f, 70.0f);
            //camera.Target = Vector3.Zero;
            //camera.Up = Vector3.Up;

            //camera.Distance = 5.0f;
            //camera.View = Matrix.Identity;
            //camera.Rotation = Quaternion.CreateFromYawPitchRoll(0, -0.3f, 0);

            //camera.Recalculate();

            //m_renderContext.Camera = camera;
        }

        private void GraphicsDeviceHost_HwndLButtonDown(object sender, HwndMouseEventArgs eventArgs)
        {
            _previousMousePosition= ConvertToVector2(eventArgs.Position);
            m_mouseDown = true;
        }

        private void GraphicsDeviceHost_HwndLButtonUp(object sender, HwndMouseEventArgs eventArgs)
        {
            m_mouseDown = false;
        }

        private Vector2 ConvertToVector2(Point point)
        {
            return new Vector2(point.X, point.Y);
        }
        private Vector2 GetMouseSpeed(Vector2 currentMousePosition)
        {
            return (_previousMousePosition - currentMousePosition);
        }
        private void GraphicsDeviceHost_HwndMouseMove(object sender, HwndMouseEventArgs eventArgs)
        {
            //if (!m_mouseDown) return;
            //var camera = m_renderContext.Camera;

            //var currentMousePosition = ConvertToVector2(eventArgs.Position);
            //var localRotation = GetMouseSpeed(currentMousePosition);
            //if (localRotation != Vector2.Zero)
            //{
            //    yawPitch += localRotation * (float)GraphicsDeviceHost.m_gameStepTimer.ElapsedTime.TotalSeconds * 0.5f;
            //    camera.Rotation = Quaternion.CreateFromYawPitchRoll(yawPitch.X, yawPitch.Y, 0);
            //}

            //camera.Recalculate();
            //_previousMousePosition = currentMousePosition;
        }
    }
}
