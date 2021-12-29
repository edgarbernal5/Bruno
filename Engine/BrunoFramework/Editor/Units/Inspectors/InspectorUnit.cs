using BrunoFramework.Editor.Game.Inspectors;
using System;

namespace BrunoFramework.Editor.Units
{
    public class InspectorUnit : EditorUnit, IInspectorService
    {
        public InspectorViewModel ViewModel => m_inspectorViewModel;

        public IInspectableObject SelectedObject 
        {
            get => m_inspectorViewModel.SelectedObject;
            set => m_inspectorViewModel.SelectedObject = value;
        }

        private InspectorViewModel m_inspectorViewModel;

        public InspectorUnit()
        {
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IInspectorService), null, this);
            m_inspectorViewModel = new InspectorViewModel(Editor);
        }

        protected override void OnStartup()
        {

        }
        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IInspectorService));
        }

        protected override void OnShutdown()
        {
            m_inspectorViewModel = null;
        }

        public override EditorDockableTabViewModel GetDockTabViewModel(string dockId)
        {
            if (dockId == InspectorViewModel.DockIdString)
                return m_inspectorViewModel;

            return null;
        }
    }
}
