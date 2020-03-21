using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Editor.Services;
using TrioWpfFramework.Windows.Docking;
using TrioWpfFramework.Windows.Docking.ViewModels;
using TrioWpfFramework.Windows.ScreenConduction;

namespace TrioWpfEditor.Editor.ViewModels
{
    internal class EditorDockTabPaneViewModel : DockTabPaneViewModel
    {
        /// <summary>
        /// Gets or sets the editor.
        /// </summary>
        /// <value>The editor.</value>
        public IEditorService Editor { get; }

        /// <summary>
        /// Initializes a new instance of the <see cref="EditorDockTabPaneViewModel" /> class.
        /// </summary>
        /// <param name="editor">The editor.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="editor"/> is <see langword="null"/>.
        /// </exception>
        public EditorDockTabPaneViewModel(IEditorService editor)
        {
            if (editor == null)
                throw new ArgumentNullException(nameof(editor));

            Editor = editor;
        }

        protected override void OnPropertyChanged(PropertyChangedEventArgs eventArgs)
        {
            base.OnPropertyChanged(eventArgs);

            if (string.IsNullOrEmpty(eventArgs.PropertyName)
                || eventArgs.PropertyName == nameof(IDockElement.DockState)
                || eventArgs.PropertyName == nameof(IDockPane.IsVisible)
                || eventArgs.PropertyName == nameof(IDockTabPane.SelectedItem))
            {
                ScreenConduction();
            }
        }


        private void ScreenConduction()
        {
            if (!IsVisible)
                return;

            // Set screen conductor.
            foreach (var item in Items)
            {
                var screen = item as IScreen;
                if (screen != null)
                    screen.Conductor = Editor;
            }

            // Note: Redundant calls of IActivatable.OnActivate/OnDeactivate should have no effect.
            if (Editor.IsActive)
            {
                // Deactivate unselected items.
                foreach (var item in Items)
                    if (item != SelectedItem && item.DockState == DockState)
                        (item as IActivatable)?.OnDeactivate(false);

                // Activate selected items.
                if (SelectedItem?.DockState == DockState)
                    (SelectedItem as IActivatable)?.OnActivate();
            }
            else
            {
                // Deactivate all items.
                foreach (var item in Items)
                    if (item.DockState == DockState)
                        (item as IActivatable)?.OnDeactivate(false);
            }
        }
    }
}