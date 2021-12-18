using System;

namespace BrunoFramework.Editor.Units
{
    public class LayoutUnit : EditorUnit
    {
        public LayoutUnit()
        {
        }

        private void Editor_Activated(object sender, ActivationEventArgs eventArgs)
        {
            if (eventArgs.WasInitialized)
            {
                Editor.Activated -= Editor_Activated;

                Editor.LoadLayout();

                Editor.Window.Closing += Window_Closing;
            }
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Editor.Window.Closing -= Window_Closing;
            Editor.SaveLayout();
        }

        protected override void OnInitialize()
        {

        }

        protected override void OnStartup()
        {
            Editor.Activated += Editor_Activated;
        }

        protected override void OnUninitialize()
        {

        }

        protected override void OnShutdown()
        {
            Editor.Activated -= Editor_Activated;
        }

        public override EditorDockableTabViewModel GetDockTabViewModel(string dockId)
        {
            return null;
        }
    }
}
