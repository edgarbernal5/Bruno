using Bruno.Collections;
using Bruno.Logging;
using Bruno.ServiceLocation;
using BrunoFramework.Editor.Game;
using BrunoFramework.Editor.Game.Inspectors;
using BrunoFramework.Editor.Units;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace BrunoFramework.Editor
{
    public class EditorViewModel : Screen, IEditorService
    {
        private static readonly ILog Logger = LogManager.GetLog();

        public event EventHandler<EventArgs> WindowActivated;

        internal static EditorViewModel DesignInstance
        {
            get
            {
                return new EditorViewModel(new ServiceContainer())
                {
                    ApplicationName = "MyEditor"
                };
            }
        }

        public SceneViewModel SceneDetail
        {
            get => m_sceneDetail;
            set
            {
                m_sceneDetail = value;
                NotifyOfPropertyChange();
            }
        }
        private SceneViewModel m_sceneDetail;

        public WorldOutlineViewModel WorldOutlineDetail
        {
            get => m_worldOutlineDetail;
            set
            {
                m_worldOutlineDetail = value;
                NotifyOfPropertyChange();
            }
        }
        private WorldOutlineViewModel m_worldOutlineDetail;

        public InspectorViewModel InspectorDetail
        {
            get => m_inspectorDetail;
            set
            {
                m_inspectorDetail = value;
                NotifyOfPropertyChange();
            }
        }
        private InspectorViewModel m_inspectorDetail;

        public string ApplicationName
        {
            get => _applicationName;
            set
            {
                _applicationName = value;
                NotifyOfPropertyChange();
            }
        }
        private string _applicationName;

        public ServiceContainer Services { get; private set; }

        public EditorUnitCollection Units { get; private set; }

        private IEnumerable<EditorUnit> OrderedUnits
        {
            get
            {
                return Units.OrderByDescending(e => e.Priority);
            }
        }

        public int ExitCode
        {
            get { return _exitCode ?? (int)Editor.ExitCode.ERROR_SUCCESS; }
        }
        private int? _exitCode;

        public EditorWindow Window
        {
            get => _window;
            internal set
            {
                _window = value;
            }
        }
        private EditorWindow _window;

        public MenuItemViewModelCollection Menu
        {
            get => _menuManager.Menu;
        }

        public List<TreeNodeCollection<ICommandItem>> MenuNodes { get; }

        private MenuManager _menuManager;
        private ResourceDictionary _resourceDictionary;

        public EditorViewModel(ServiceContainer serviceContainer)
        {
            Services = serviceContainer;
            Units = new EditorUnitCollection();
            MenuNodes = new List<TreeNodeCollection<ICommandItem>>();
            _menuManager = new MenuManager();
        }

        public void Configure()
        {
            Logger.Info("Configuring editor view model");

            DisplayName = "Estero Editor";

            Services.RegisterInstance(typeof(IEditorService), null, this);
            Services.RegisterPerRequest(typeof(IViewLocator), null, typeof(EditorViewLocator));
            Services.RegisterView(typeof(EditorViewModel), typeof(EditorWindow));

            _resourceDictionary = new ResourceDictionary { Source = new Uri("pack://application:,,,/BrunoFramework;component/Resources/EditorDataTemplates.xaml", UriKind.RelativeOrAbsolute) };
            EditorHelper.RegisterResources(_resourceDictionary);

            foreach (var unit in OrderedUnits)
            {
                unit.Initialize(this);
            }
        }

        public void Startup()
        {
            Logger.Info("Starting editor view model");

            foreach (var unit in OrderedUnits)
            {
                unit.Startup();
            }

            LoadLayout();
            CreateEmptyScene();

            RecreateUI();
        }

        private void LoadLayout()
        {
            var worldOutlineService = Services.GetInstance<IWorldOutlineService>();
            WorldOutlineDetail = worldOutlineService.ViewModel;

            var inspectorService = Services.GetInstance<IInspectorService>();
            InspectorDetail = inspectorService.ViewModel;
        }

        private void CreateEmptyScene()
        {
            var projectFileService = Services.GetInstance<IProjectFileService>();

            var sceneFileType = projectFileService.Factories.SelectMany(factory => factory.SupportedFileTypes)
                .Where(fileType => fileType.Name == "Scene").FirstOrDefault();

            projectFileService.New(sceneFileType);
        }

        private void RecreateUI()
        {
            Logger.Debug("Recreating UI");

            _menuManager.Update(MenuNodes);
        }

        public void Shutdown()
        {
            Logger.Info("Shutting down editor view model");

            foreach (var unit in OrderedUnits.Reverse())
            {
                unit.Shutdown();
            }

            foreach (var unit in OrderedUnits.Reverse())
            {
                unit.Uninitialize();
            }

            EditorHelper.UnregisterResources(_resourceDictionary);
        }

        public void Exit(int exitCode = (int)Editor.ExitCode.ERROR_SUCCESS)
        {
            if (!_exitCode.HasValue)
            {
                _exitCode = exitCode;
            }
            _window.Close();
        }

        public void ActivateItem(object item)
        {
            if (item is SceneViewModel)
            {
                SceneDetail = item as SceneViewModel;
            }

            var activate = item as IActivate;
            if (activate != null)
            {
                activate.Activate();
            }
        }
    }
}
