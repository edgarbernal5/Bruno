
using Bruno.Logging;
using Bruno.ServiceLocation;
using BrunoFramework;
using BrunoFramework.Editor;
using BrunoFramework.Editor.Units;
using System.Windows;

namespace BrunoWpfEditor
{
    public class Bootstrapper : BootstrapperBase
    {
        private static readonly ILog Logger;

        private ServiceContainer _serviceContainer;
        private EditorViewModel _editor;

        public Bootstrapper()
        {
            Initialize();
        }

        static Bootstrapper()
        {
            LogManager.Targets = name => new CompositeLog(new ILog[] { new DebugLog() });
            Logger = LogManager.GetLog();
        }

        protected override void OnConfigure()
        {
            Logger.Info("Configuring editor");

            _serviceContainer = new ServiceContainer();
            _serviceContainer.RegisterPerRequest(typeof(IWindowManager), null, typeof(WindowManager));

            _editor = new EditorViewModel(_serviceContainer);

            //Core units
            _editor.Units.Add(new MenuUnit());
            _editor.Units.Add(new BuilderUnit());
            _editor.Units.Add(new GameUnit());
            _editor.Units.Add(new PrimitivesUnit());
            _editor.Units.Add(new WorldOutlineUnit());
            _editor.Units.Add(new DocumentUnit());
            _editor.Units.Add(new InspectorUnit());
            _editor.Units.Add(new ContentBrowserUnit());

            _editor.Configure();
        }

        protected override void OnStartup(object sender, StartupEventArgs eventArgs)
        {
            Logger.Info("Configuring startup");
            _editor.Startup();

            var windowService = _serviceContainer.GetInstance<IWindowManager>();
            windowService.ShowWindow(_editor, null, false);
        }

        protected override void OnExit(object sender, ExitEventArgs eventArgs)
        {
            Logger.Info("Exiting application");

            _editor.Shutdown();
            _serviceContainer.Dispose();
        }
    }

    //GraphicsDeviceService.Service.DeviceCreated += Service_DeviceCreated;
    //    private void Service_DeviceCreated(object sender, EventArgs e)
    //    {
    //        var device = GraphicsDeviceService.Service.GraphicsDevice;
    //        var ibuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, 36, ResourceUsage.Immutable);

    //        uint[] indices = new uint[]{
    //                // front face
    //                0, 1, 2,
    //                0, 2, 3,

    //                // back face
    //                4, 6, 5,
    //                4, 7, 6,

    //                // left face
    //                4, 5, 1,
    //                4, 1, 0,

    //                // right face
    //                3, 2, 6,
    //                3, 6, 7,

    //                // top face
    //                1, 5, 6,
    //                1, 6, 2,

    //                // bottom face
    //                4, 0, 3,
    //                4, 3, 7
    //            };

    //        ibuffer.SetData<uint>(indices);
    //        Effect effect = new Effect(device, @"Effects\GizmoModel.fx");

    //        Scene scene = new Scene();

    //        GameObject gameObject = GameObject.Create("GO1");
    //        string name = gameObject.Name;
    //        int id = gameObject.Id;

    //        BuildCoordinatorSettings settings = new BuildCoordinatorSettings();
    //        //settings.IntermediateDirectory = "";
    //        //settings.OutputDirectory = "";
    //        //settings.RootDirectory = "";

    //        BuildCoordinator buildCoordinator = new BuildCoordinator(settings);
    //    }
}
