using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using TrioWpfFramework.Editor;
using TrioWpfFramework.Editor.Commands;
using TrioWpfFramework.Editor.Themes;
using TrioWpfFramework.ServiceLocation;
using TrioWpfFramework.Windows;
using TrioWpfFramework.Windows.Framework;

namespace TrioWpfEditor
{
    public class AppBootstrapper : Bootstrapper
    {
        //private const string UniqueName = "DigitalRune_EditorApp";
        private const string ApplicationName = "EditorApp";
        private const int ExitCodeConfigurationFailed = 1;
        private const string Email = "office@digitalrune.com";

        //private static readonly Logger Logger = LogManager.GetCurrentClassLogger();

        private ServiceContainer _serviceContainer;
        private EditorViewModel _editor;
        private bool _configurationFailed;
        //private DocumentExtension _documentExtension;

        protected override void OnConfigure()
        {
            if (WpfWindowsExtensions.IsInDesignMode)
                return;

            //Logger.Info("Configuring editor.");

#if !DEBUG
            PresentationTraceSources.DataBindingSource.Switch.Level = SourceLevels.Critical;
#endif

            _serviceContainer = new ServiceContainer();

            // Configure general services.
            _serviceContainer.Register(typeof(IWindowService), null, typeof(WindowManager));

            // Configure editor.
            _editor = new EditorViewModel(_serviceContainer)
            {
                ApplicationName = ApplicationName,
                ApplicationIcon = BitmapFrame.Create(new Uri("pack://application:,,,/TrioWpfEditor;component/Resources/Estero.ico", UriKind.RelativeOrAbsolute))
            };
            // Core extensions
            _editor.Plugins.Add(new CommandPlugin());
            //_editor.Extensions.Add(new LayoutExtension());
            //_editor.Extensions.Add(new AboutExtension());
            //_editor.Extensions.Add(new OptionsExtension());
            //_editor.Extensions.Add(new PrintExtension());
            //_editor.Extensions.Add(new QuickLaunchExtension());
            _editor.Plugins.Add(new ThemePlugin());
            //_editor.Extensions.Add(new StatusExtension());
            //_editor.Extensions.Add(new SearchExtension());

            //// Common tool windows.
            //_editor.Extensions.Add(new OutputExtension());
            //_editor.Extensions.Add(new ErrorExtension());
            //_editor.Extensions.Add(new OutlineExtension());
            //_editor.Extensions.Add(new PropertiesExtension());

            //// Document extensions.
            //_documentExtension = new DocumentExtension();
            //_editor.Extensions.Add(_documentExtension);
            //_editor.Extensions.Add(new TextExtension());
            //_editor.Extensions.Add(new ShaderExtension());
            //_editor.Extensions.Add(new TexturesExtension());
            //_editor.Extensions.Add(new ModelsExtension());

            //// Other extensions.
            //_editor.Extensions.Add(new DiagnosticsExtension());
            //_editor.Extensions.Add(new ColorExtension());
            //_editor.Extensions.Add(new GameExtension());
            //_editor.Extensions.Add(new TestExtension0());

            try
            {
                bool success = _editor.Initialize();
                if (!success)
                {
                    // The editor could not be configured or command line arguments caused the
                    // editor to shut down (e.g. if "--help" was specified).
                    Application.Shutdown(_editor.ExitCode);
                    return;
                }
            }
            catch (Exception exception)
            {
                //Logger.Error(exception, "Editor configuration failed.");

                _configurationFailed = true;
                Environment.ExitCode = ExitCodeConfigurationFailed;

                ExceptionHelper.ShowException(exception, ApplicationName, Email);
            }
        }


        protected override void OnStartup(object sender, StartupEventArgs eventArgs)
        {
            if (WpfWindowsExtensions.IsInDesignMode)
                return;

            if (_configurationFailed)
            {
                // The editor configuration has failed.
                // The Application is running and waits until the Exception window is closed.
                return;
            }

            //bool isFirstInstance = SingleInstanceApplication.Initialize(UniqueName, OnOtherInstanceStarted);
            //if (!isFirstInstance)
            //{
            //    // If the command-line arguments are empty, then start new application instance.
            //    // But if a "file" was specified on the command-line, then open the document in
            //    // the first instance.
            //    if (_editor.CommandLineResult.ParsedArguments["file"] != null)
            //    {
            //        // Notify first instance.
            //        SingleInstanceApplication.SignalFirstInstance();

            //        // Shutdown application.
            //        Application.Shutdown(_editor.ExitCode);
            //        return;
            //    }
            //}

            // Start editor.
            bool success = _editor.Startup();
            if (!success)
            {
                Application.Shutdown(_editor.ExitCode);
                return;
            }

            // Set About dialog information.
            //var aboutService = _serviceContainer.GetInstance<IAboutService>();
            //if (aboutService != null)
            //{
            //    aboutService.Copyright = "© 2008-2016 DigitalRune GmbH";
            //    aboutService.Icon = _editor.ApplicationIcon;
            //    var additionalInfo = new TextBlock();
            //    additionalInfo.Inlines.Add(new Run("Additional information and support: "));
            //    var link = new Hyperlink(new Run("http://www.digitalrune.com/"))
            //    {
            //        NavigateUri = new Uri("http://www.digitalrune.com/")
            //    };
            //    link.Click += delegate { Process.Start(new ProcessStartInfo(link.NavigateUri.ToString())); };
            //    additionalInfo.Inlines.Add(link);
            //    aboutService.Information = additionalInfo;
            //    aboutService.InformationAsString = "Additional information and support: http://www.digitalrune.com/";
            //}

            // Show window.
            var windowService = _serviceContainer.GetInstance<IWindowService>().ThrowIfMissing();
            windowService.ShowWindow(_editor, null, false);
        }

        protected override void OnExit(object sender, ExitEventArgs eventArgs)
        {
            //Logger.Info("Exiting application.");

            // Clean up.
            _editor.Shutdown();
            _serviceContainer.Dispose();
            //SingleInstanceApplication.Cleanup();

            // Set application's exit code.
            eventArgs.ApplicationExitCode = _editor.ExitCode;
        }


        protected override void OnUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs eventArgs)
        {
            // An unhandled exception. Report the exception and then exit.
            //Logger.Error(eventArgs.Exception, "Unhandled exception.");

            ExceptionHelper.ShowException(eventArgs.Exception, ApplicationName, Email);

            Application.Shutdown();
            eventArgs.Handled = true;
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
