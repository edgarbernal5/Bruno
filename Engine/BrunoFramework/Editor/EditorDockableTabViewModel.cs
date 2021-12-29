
using System.Windows.Input;

namespace BrunoFramework.Editor
{
    public abstract class EditorDockableTabViewModel : Screen
    {
        public string Title
        {
            get => DisplayName;
        }

        public string DockId { get; protected set; }

        public ICommand CloseCommand
        {
            get
            {
                if (m_closeCommand == null)
                {
                    m_closeCommand = new DelegateCommand(CloseTab, () =>
                    {
                        var canClose = CanCloseAsync();
                        return canClose.Result;
                    });
                }
                return m_closeCommand;
            }
        }
        private ICommand m_closeCommand;

        protected void CloseTab()
        {
            ((IDeactivate)this).Deactivate(true);
        }
    }
}
