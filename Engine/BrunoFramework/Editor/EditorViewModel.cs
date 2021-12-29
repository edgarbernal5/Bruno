
using AvalonDock;
using Bruno.Collections;
using Bruno.Logging;
using Bruno.ServiceLocation;
using BrunoFramework.Editor.Units;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Windows;

namespace BrunoFramework.Editor
{
    public class EditorViewModel : Screen, IEditorService
    {
        private static readonly ILog Logger = LogManager.GetLog();

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

        public ReadOnlyObservableCollection<EditorDockableTabViewModel> Tabs
        {
            get
            {
                if (_readonyTabs == null)
                    _readonyTabs = new ReadOnlyObservableCollection<EditorDockableTabViewModel>(_tabs);

                return _readonyTabs;
            }
        }

        public DockingManager DockManager
        {
            get => m_dockManager;
            set => m_dockManager = value;
        }

        public DocumentViewModel ActiveDocument
        {
            get => m_activeDocument;
            set
            {
                m_activeDocument = value;
                NotifyOfPropertyChange();
                if (ActiveDocumentChanged != null) 
                    ActiveDocumentChanged(this, EventArgs.Empty);
            }
        }
        private DocumentViewModel m_activeDocument;

        private DockingManager m_dockManager;

        private ObservableCollection<EditorDockableTabViewModel> _tabs = new ObservableCollection<EditorDockableTabViewModel>();
        private ReadOnlyObservableCollection<EditorDockableTabViewModel> _readonyTabs = null;

        public event EventHandler<EventArgs> ActiveDocumentChanged;

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

            DisplayName = "Bruno Editor";

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

            CreateEmptyScene();

            RecreateUI();
        }

        protected override void OnActivate()
        {

        }
        public void LoadLayout()
        {
            _tabs.Clear();

            var serializer = new AvalonDock.Layout.Serialization.XmlLayoutSerializer(m_dockManager);
            serializer.LayoutSerializationCallback += (sender, args) =>
            {
                var dockId = args.Model.ContentId;
                var viewModel = Units.Select(unit => unit.GetDockTabViewModel(dockId))
                                    .Where(vm => vm != null)
                                    .FirstOrDefault();

                if (viewModel == null)
                {
                    args.Cancel = true;
                }
                else
                {
                    args.Content = viewModel;
                    viewModel.Deactivated += (senderVM, eventArgs) =>
                    {
                        _tabs.Remove(viewModel);
                    };

                    ((IActivate)viewModel).Activate();
                    _tabs.Add(viewModel);
                }
            };

            if (File.Exists(@".\AvalonDock.config"))
                serializer.Deserialize(@".\AvalonDock.config");

            //var worldOutlineService = Services.GetInstance<IWorldOutlineService>();
            //_tabs.Add(worldOutlineService.ViewModel);

            //var inspectorService = Services.GetInstance<IInspectorService>();
            //_tabs.Add(inspectorService.ViewModel);

            //var contentBrowserService = Services.GetInstance<IContentBrowserService>();
            //_tabs.Add(contentBrowserService.ViewModel);
        }

        public void SaveLayout()
        {
            var serializer = new AvalonDock.Layout.Serialization.XmlLayoutSerializer(m_dockManager);
            serializer.Serialize(@".\AvalonDock.config");
        }

        private void CreateEmptyScene()
        {
            var documentService = Services.GetInstance<IDocumentService>();

            var documentType = documentService.Factories.SelectMany(factory => factory.SupportedFileTypes)
                .Where(fileType => fileType.Name == "Scene").FirstOrDefault();

            //documentService.New(documentType);
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
            if (item is EditorDockableTabViewModel)
            {
                var editorTab = item as EditorDockableTabViewModel;
                if (!editorTab.IsActive)
                {
                    editorTab.Deactivated += (senderVM, eventArgs) =>
                    {
                        _tabs.Remove(editorTab);
                    };
                    _tabs.Add(editorTab);
                }
            }
            if (item is DocumentViewModel)
            {
                ActiveDocument = item as DocumentViewModel;
            }

            var activate = item as IActivate;
            if (activate != null)
            {
                activate.Activate();
            }
        }
    }
}
