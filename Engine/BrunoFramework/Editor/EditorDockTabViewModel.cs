
using System.Windows.Input;

namespace BrunoFramework.Editor
{
    public abstract class EditorDockTabViewModel : Screen, IEditorDockTab
    {
        public string Title => DisplayName;

        public string DockId { get; protected set; }

        public ICommand CloseCommand
        {
            get
            {
                if (m_closeCommand == null)
                {
                    m_closeCommand = new AsyncCommand(() =>
                    {
                        return TryCloseAsync();
                    });
                }
                return m_closeCommand;
            }
        }
        private ICommand m_closeCommand;

        public bool IsSelected
        {
            get => m_isSelected;
            set
            {
                m_isSelected = value;
                NotifyOfPropertyChange();
            }
        }
        private bool m_isSelected;
    }
}
