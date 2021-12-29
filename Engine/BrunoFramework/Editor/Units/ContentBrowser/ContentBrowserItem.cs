
using BrunoWindows;
using System.Windows.Media;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserItem : PropertyChangedBase
    {
        public string Text
        {
            get => m_text;
            set
            {
                m_text = value;
                NotifyOfPropertyChange();
            }
        }
        private string m_text;

        public ImageSource Icon
        {
            get => m_icon;
            set
            {
                m_icon = value;
                NotifyOfPropertyChange();
            }
        }
        private ImageSource m_icon;
    }
}
