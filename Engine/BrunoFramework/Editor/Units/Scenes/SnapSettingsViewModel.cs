
using BrunoFramework.Editor.Game.Gizmos;
using System.Windows.Input;

namespace BrunoFramework.Editor.Units
{
    public class SnapSettingsViewModel : Dialog
    {
        public GizmoSnappingConfig Config { get; set; }

        public ICommand AcceptCommand
        {
            get
            {
                if (m_acceptCommand == null)
                {
                    m_acceptCommand = new DelegateCommand(() => { DialogResult = true; });
                }
                return m_acceptCommand;
            }
        }
        private ICommand m_acceptCommand;

        public ICommand CancelCommand
        {
            get
            {
                if (m_cancelCommand == null)
                {
                    m_cancelCommand = new DelegateCommand(() => { DialogResult = false; });
                }
                return m_cancelCommand;
            }
        }
        private ICommand m_cancelCommand;
    }
}
