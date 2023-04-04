
namespace BrunoFramework
{
    public class Dialog : Screen, IDialogResult
    {
        public bool? DialogResult
        {
            get => m_dialogResult;
            set
            {
                m_dialogResult = value;
                NotifyOfPropertyChange();
            }
        }
        private bool? m_dialogResult;
    }
}
