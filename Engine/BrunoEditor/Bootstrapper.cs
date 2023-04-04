
using Bruno.Logging;
using Bruno.ServiceLocation;
using BrunoFramework;
using BrunoFramework.Editor;
using BrunoFramework.Editor.Units;
using System.Windows;
using System.Windows.Threading;

namespace BrunoEditor
{
    public class Bootstrapper : BootstrapperBase
    {
        private static readonly ILog Logger;

        private ServiceContainer m_serviceContainer;
        private EditorViewModel m_editor;

        public Bootstrapper()
        {
            Initialize();
        }

        static Bootstrapper()
        {
            var globalLogTargets = new CompositeLog(new ILog[] { new InMemoryLog() });
            //var globalLogTargets = new CompositeLog(new ILog[] { new DebugLog(), new InMemoryLog() });
            Bruno.Logging.Logger.Targets = name => globalLogTargets;

            Logger = Bruno.Logging.Logger.GetLog();
        }

        protected override void OnConfigure()
        {
            Logger.Info("Configuring editor");

            m_serviceContainer = new ServiceContainer();
            m_serviceContainer.RegisterPerRequest(typeof(IWindowManager), typeof(WindowManager));

            m_editor = new EditorViewModel(m_serviceContainer);

            //Core units
            m_editor.Units.Add(new MenuUnit());
            m_editor.Units.Add(new LayoutUnit());
            m_editor.Units.Add(new BuilderUnit());
            m_editor.Units.Add(new GameUnit());
            m_editor.Units.Add(new PrimitivesUnit());
            m_editor.Units.Add(new WorldOutlineUnit());
            m_editor.Units.Add(new DocumentUnit());
            m_editor.Units.Add(new PropertiesUnit());
            m_editor.Units.Add(new ContentBrowserUnit());
            m_editor.Units.Add(new ConsoleUnit());
            m_editor.Units.Add(new SceneUnit());

            m_editor.Configure();
        }

        protected override void OnStartup(object sender, StartupEventArgs eventArgs)
        {
            Logger.Info("Configuring startup");
            m_editor.Startup();

            var windowService = m_serviceContainer.GetInstance<IWindowManager>();
            windowService.ShowWindow(m_editor, null, false);
        }

        protected override void OnExit(object sender, ExitEventArgs eventArgs)
        {
            Logger.Info("Exiting application");

            m_editor.Shutdown();
            m_serviceContainer.Dispose();
        }

        protected override void OnUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            Logger.Info("OnUnhandledException");
        }
    }
}
