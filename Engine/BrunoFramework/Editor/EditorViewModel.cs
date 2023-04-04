
using AvalonDock;
using Bruno.Collections;
using Bruno.Logging;
using Bruno.ServiceLocation;
using BrunoFramework.Editor.Game;
using BrunoFramework.Editor.Units;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace BrunoFramework.Editor
{
    public class EditorViewModel : OneActiveConductor<Screen>, IEditorService
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

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
            get => m_window;
            internal set
            {
                m_window = value;
            }
        }
        private EditorWindow m_window;

        public MenuItemViewModelCollection Menu
        {
            get => m_menuManager.Menu;
        }
        private MenuManager m_menuManager;

        public List<TreeNodeCollection<ICommandItem>> MenuNodes { get; }

        private ResourceDictionary m_resourceDictionary;

        public DockingManager DockManager
        {
            get => m_dockManager;
            set => m_dockManager = value;
        }
        private DockingManager m_dockManager;

        public event EventHandler<EventArgs> ActiveItemChanged;

        public EditorViewModel(ServiceContainer serviceContainer)
        {
            Services = serviceContainer;
            Units = new EditorUnitCollection();
            MenuNodes = new List<TreeNodeCollection<ICommandItem>>();
            m_menuManager = new MenuManager();
        }

        private void OnEditorPropertyChanged(object sender, PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs.PropertyName == nameof(ActiveItem))
            {
                ActiveItemChanged?.Invoke(this, EventArgs.Empty);
            }
        }

        public void Configure()
        {
            Logger.Info("Configuring editor view model");

            DisplayName = "Bruno Editor";

            Services.RegisterInstance(typeof(IEditorService), this);
            Services.RegisterPerRequest(typeof(IViewLocator), typeof(EditorViewLocator));
            Services.RegisterView(typeof(EditorViewModel), typeof(EditorWindow));

            m_resourceDictionary = new ResourceDictionary { Source = new Uri("pack://application:,,,/BrunoFramework;component/Resources/EditorDataTemplates.xaml", UriKind.RelativeOrAbsolute) };
            EditorHelper.RegisterResources(m_resourceDictionary);

            foreach (var unit in OrderedUnits)
            {
                unit.Initialize(this);
            }

            PropertyChanged += OnEditorPropertyChanged;
        }

        public void Startup()
        {
            Logger.Info("Starting editor view model");

            foreach (var unit in OrderedUnits)
            {
                unit.Startup();
            }

            RecreateUI();
        }

        public void LoadLayout()
        {
            Screen lastTab = null;
            var serializer = new AvalonDock.Layout.Serialization.XmlLayoutSerializer(m_dockManager);
            serializer.LayoutSerializationCallback += async (sender, args) =>
            {
                var dockId = args.Model.ContentId;
                var viewModel = Units.Select(unit => unit.GetDockTabViewModel(dockId))
                                    .FirstOrDefault(vm => vm != null);

                if (viewModel == null)
                {
                    args.Cancel = true;
                }
                else
                {
                    args.Content = viewModel;

                    viewModel.Conductor = this;
                    await ActivateItemAsync(viewModel, CancellationToken.None);

                    lastTab = viewModel;
                }
            };

            try
            {
                if (File.Exists(@".\Layout.config"))
                {
                    serializer.Deserialize(@".\Layout.config");
                }

                var sceneView = Items.OfType<SceneDocumentViewModel>().FirstOrDefault();
                if (sceneView == null)
                {
                    var documentService= Services.GetInstance<IDocumentService>();
                    var selectedDocumentType = documentService.Factories.SelectMany(factory => factory.SupportedFileTypes)
                        .FirstOrDefault(fileType => fileType.Name == "Scene");

                    documentService.New(selectedDocumentType);
                }

                if (lastTab != null)
                {
                    ActiveItem = lastTab;
                }
            }
            catch (Exception ex)
            {
                Logger.Error(ex);
            }
        }

        public void SaveLayout()
        {
            try
            {
                var serializer = new AvalonDock.Layout.Serialization.XmlLayoutSerializer(m_dockManager);
                serializer.Serialize(@".\Layout.config");
            }
            catch (Exception ex)
            {
                Logger.Error(ex);
            }
        }

        private void RecreateUI()
        {
            Logger.Debug("Recreating UI");

            m_menuManager.Update(MenuNodes);
        }

        public void Shutdown()
        {
            Logger.Info("Shutting down editor view model");
            PropertyChanged -= OnEditorPropertyChanged;

            foreach (var unit in OrderedUnits.Reverse())
            {
                unit.Shutdown();
            }

            foreach (var unit in OrderedUnits.Reverse())
            {
                unit.Uninitialize();
            }

            EditorHelper.UnregisterResources(m_resourceDictionary);
        }

        public void Exit(int exitCode = (int)Editor.ExitCode.ERROR_SUCCESS)
        {
            if (!_exitCode.HasValue)
            {
                _exitCode = exitCode;
            }
            m_window.Close();
        }

        public void ActivateItem(Screen item)
        {
            item.Conductor = this;
            Task.Run(() => ActivateItemAsync(item, CancellationToken.None));
        }

        public override async Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            foreach (var unitGuardClose in OrderedUnits.OfType<IGuardClose>())
            {
                var canClose = await unitGuardClose.CanCloseAsync(cancellationToken);
                if (!canClose)
                {
                    return false;
                }
            }

            return await base.CanCloseAsync();
        }
    }
}
