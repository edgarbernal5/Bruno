
using BrunoFramework.Editor.Game.Gizmos;
using System.Threading.Tasks;

namespace BrunoFramework.Editor.Units
{
    public class SceneUnit : EditorUnit, ISceneService
    {
        private SceneDocumentFactory m_factory;
        private IDocumentService m_documentService;
        private IWindowManager m_windowManager;


        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(ISceneService), this);
            Editor.Services.RegisterView(typeof(SnapSettingsViewModel), typeof(SnapSettingsView));
            m_documentService = Editor.Services.GetInstance<IDocumentService>();
            m_windowManager = Editor.Services.GetInstance<IWindowManager>();

            m_factory = new SceneDocumentFactory(Editor);
            m_documentService.Factories.Add(m_factory);
        }

        protected override void OnStartup()
        {
        }

        protected override void OnShutdown()
        {
        }

        protected override void OnUninitialize()
        {
            m_documentService.Factories.Remove(m_factory);
            m_factory = null;
        }

        public async Task ShowSnapSettingsAsync(GizmoSnappingConfig snappingConfig)
        {
            var snapSettingsViewModel = new SnapSettingsViewModel()
            {
                Config = snappingConfig.Clone()
            };
            var result = await m_windowManager.ShowDialog(snapSettingsViewModel);
            if (result.HasValue && result.Value)
            {
                snappingConfig.CopyFrom(snapSettingsViewModel.Config);
            }
        }
    }
}
