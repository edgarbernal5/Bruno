using System;
using TrioWpfFramework.Editor.Commands;
using TrioWpfFramework.Editor.Services;
using TrioWpfFramework.Windows.Docking;
using static System.FormattableString;

namespace TrioWpfEditor.Editor
{
    public abstract class EditorPlugin
    {
        private bool _initialized;   // True if Initialize and Startup were executed.

        public IEditorService Editor { get; private set; }

        public CommandItemCollection CommandItems { get; } = new CommandItemCollection();

        public int Priority { get; set; }

        protected EditorPlugin()
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Creating {0}.", GetType().Name);
        }

        internal void Initialize(IEditorService editor)
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Initializing {0}.", GetType().Name);

            if (editor == null)
                throw new ArgumentNullException(nameof(editor));

            Editor = editor;
            OnInitialize();
        }

        protected abstract void OnInitialize();

        internal void Startup()
        {
            if (_initialized)
            {
                var message = Invariant($"Cannot startup extension {GetType().Name}. Extension has already been initialized.");
                throw new InvalidOperationException(message);
            }

            //Logger.Debug(CultureInfo.InvariantCulture, "Starting {0}.", GetType().Name);
            OnStartup();
            _initialized = true;
        }


        protected abstract void OnStartup();

        internal void Shutdown()
        {
            // Only shut down the extension if it has Startup() has been called previously.
            // (Note: It is okay to call Shutdown() without previously calling Startup().)
            if (!_initialized)
                return;

            //Logger.Debug(CultureInfo.InvariantCulture, "Shutting down {0}.", GetType().Name);
            OnShutdown();
            _initialized = false;
        }

        protected abstract void OnShutdown();

        internal void Uninitialize()
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Uninitializing {0}.", GetType().Name);

            OnUninitialize();
            Editor = null;
        }

        protected abstract void OnUninitialize();

        internal IDockTabItem GetViewModel(string dockId)
        {
            if (string.IsNullOrEmpty(dockId))
                return null;

            return OnGetViewModel(dockId);
        }

        protected abstract IDockTabItem OnGetViewModel(string dockId);
    }
}
