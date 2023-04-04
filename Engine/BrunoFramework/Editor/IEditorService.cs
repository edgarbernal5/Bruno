using AvalonDock;
using Bruno.Collections;
using Bruno.ServiceLocation;
using BrunoFramework.Editor.Units;
using System;
using System.Collections.Generic;

namespace BrunoFramework.Editor
{
    public interface IEditorService : IActivate, IDeactivate, IGuardClose, IHaveActiveItem<Screen>
    {
        string ApplicationName { get; set; }

        ServiceContainer Services { get; }

        EditorUnitCollection Units { get; }

        void Exit(int exitCode = (int)Editor.ExitCode.ERROR_SUCCESS);

        int ExitCode { get; }

        EditorWindow Window { get; }

        MenuItemViewModelCollection Menu { get; }

        List<TreeNodeCollection<ICommandItem>> MenuNodes { get; }

        DockingManager DockManager { get; set; }

        void LoadLayout();

        void SaveLayout();

        void ActivateItem(Screen item);

        event EventHandler<EventArgs> ActiveItemChanged;
    }
}
