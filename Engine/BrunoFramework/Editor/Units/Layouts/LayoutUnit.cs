
namespace BrunoFramework.Editor.Units
{
    public class LayoutUnit : EditorUnit
    {
        public LayoutUnit()
        {
        }

        private void OnEditorActivated(object sender, ActivationEventArgs eventArgs)
        {
            if (eventArgs.WasInitialized)
            {
                Editor.Activated -= OnEditorActivated;

                Editor.LoadLayout();

                Editor.Window.Closing += OnWindowClosing;
            }
        }

        private void OnWindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            //Editor.Window.Closing -= OnWindowClosing;
            Editor.SaveLayout();
        }

        protected override void OnInitialize()
        {
        }

        protected override void OnStartup()
        {
            Editor.Activated += OnEditorActivated;
        }

        protected override void OnUninitialize()
        {
        }

        protected override void OnShutdown()
        {
            Editor.Activated -= OnEditorActivated;
        }
    }
}
