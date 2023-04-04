
using Bruno.Logging;
using System;

namespace BrunoFramework.Editor
{
    public abstract class EditorUnit
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        public int Priority { get; set; }

        public IEditorService Editor { get; private set; }

        public CommandItemCollection CommandItems { get; private set; }

        private bool m_initialized;

        protected EditorUnit()
        {
            Logger.Debug("Creating {0}.", GetType().Name);

            CommandItems = new CommandItemCollection();
        }

        public void Initialize(IEditorService editor)
        {
            Logger.Debug("Initializing {0}.", GetType().Name);

            Editor = editor;
            OnInitialize();
        }

        protected abstract void OnInitialize();

        public void Startup()
        {
            if (m_initialized)
            {
                throw new InvalidOperationException("Core unit has already been initialized");
            }

            OnStartup();
            m_initialized = true;
        }

        protected abstract void OnStartup();

        public void Shutdown()
        {
            if (!m_initialized)
                return;

            Logger.Debug("Shutting down {0}.", GetType().Name);
            OnShutdown();
            m_initialized = false;
        }

        protected abstract void OnShutdown();

        public void Uninitialize()
        {
            Logger.Debug("Uninitializing {0}.", GetType().Name);

            OnUninitialize();
            Editor = null;
        }

        protected abstract void OnUninitialize();

        public virtual EditorDockTabViewModel GetDockTabViewModel(string dockId)
        {
            return default(EditorDockTabViewModel);
        }
    }
}
