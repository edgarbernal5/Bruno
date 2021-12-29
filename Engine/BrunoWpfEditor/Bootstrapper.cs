
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
            _editor.Units.Add(new LayoutUnit());
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
}
