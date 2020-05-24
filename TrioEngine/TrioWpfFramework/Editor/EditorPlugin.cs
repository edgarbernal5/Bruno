using System;
using TrioWpfFramework.Windows.Docking;
using static System.FormattableString;

namespace TrioWpfFramework.Editor
{
    public abstract class EditorPlugin
    {
        private bool _initialized;   // True if Initialize and Startup were executed.

        /// <summary>
        /// Gets the editor.
        /// </summary>
        /// <value>The editor.</value>
        public IEditorService Editor { get; private set; }

        /// <summary>
        /// Gets or sets the command items provided by this plugin.
        /// </summary>
        /// <value>The command items.</value>
        /// <remarks>
        /// Extensions can use custom command items to create new menus and toolbars. All items
        /// should be registered in this collection.
        /// </remarks>
        public CommandItemCollection CommandItems { get; } = new CommandItemCollection();

        /// <summary>
        /// Gets or sets the priority.
        /// </summary>
        /// <value>The priority.</value>
        /// <remarks>
        /// The editor sorts <see cref="EditorExtension"/> by their priority. Extensions with a
        /// higher priority are initialized first.
        /// </remarks>
        public int Priority { get; set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="EditorExtension"/> class.
        /// </summary>
        protected EditorPlugin()
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Creating {0}.", GetType().Name);
        }

        /// <summary>
        /// Initializes the editor plugin.
        /// </summary>
        /// <param name="editor">The editor.</param>
        internal void Initialize(IEditorService editor)
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Initializing {0}.", GetType().Name);

            if (editor == null)
                throw new ArgumentNullException(nameof(editor));

            Editor = editor;
            OnInitialize();
        }

        /// <summary>
        /// Called when the plugin should be initialized.
        /// </summary>
        protected abstract void OnInitialize();

        /// <summary>
        /// Initializes the editor plugin.
        /// </summary>
        /// <exception cref="InvalidOperationException">
        /// The editor plugin has already been initialized.
        /// </exception>
        internal void Startup()
        {
            if (_initialized)
            {
                var message = Invariant($"Cannot startup plugin {GetType().Name}. Extension has already been initialized.");
                throw new InvalidOperationException(message);
            }

            //Logger.Debug(CultureInfo.InvariantCulture, "Starting {0}.", GetType().Name);
            OnStartup();
            _initialized = true;
        }

        /// <summary>
        /// Called when the plugin needs to be initialized.
        /// </summary>
        /// <inheritdoc cref="OnInitialize"/>
        protected abstract void OnStartup();

        /// <summary>
        /// Shuts down this plugin.
        /// </summary>
        internal void Shutdown()
        {
            // Only shut down the plugin if it has Startup() has been called previously.
            // (Note: It is okay to call Shutdown() without previously calling Startup().)
            if (!_initialized)
                return;

            //Logger.Debug(CultureInfo.InvariantCulture, "Shutting down {0}.", GetType().Name);
            OnShutdown();
            _initialized = false;
        }

        /// <summary>
        /// Called when the plugin needs to be de-initialized.
        /// </summary>
        protected abstract void OnShutdown();

        /// <summary>
        /// De-initializes the editor plugin.
        /// </summary>
        internal void Uninitialize()
        {
            //Logger.Debug(CultureInfo.InvariantCulture, "Uninitializing {0}.", GetType().Name);

            OnUninitialize();
            Editor = null;
        }

        /// <summary>
        /// Called when the plugin should be uninitialized.
        /// </summary>
        protected abstract void OnUninitialize();

        /// <summary>
        /// Gets the <see cref="IDockTabItem"/> for a given ID.
        /// </summary>
        /// <param name="dockId">
        /// The ID of the <see cref="IDockTabItem"/> (see <see cref="IDockTabItem.DockId"/>).
        /// </param>
        /// <returns>
        /// The view model that matches the given ID, or <see langword="null"/> if the ID is
        /// unknown.
        /// </returns>
        /// <inheritdoc cref="OnGetViewModel" />
        internal IDockTabItem GetViewModel(string dockId)
        {
            if (string.IsNullOrEmpty(dockId))
                return null;

            return OnGetViewModel(dockId);
        }

        protected abstract IDockTabItem OnGetViewModel(string dockId);
    }
}
