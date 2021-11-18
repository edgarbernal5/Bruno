
using BrunoWindows;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserFileListItem : PropertyChangedBase
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

        public string ImageFilename
        {
            get => m_imageFilename;
            set
            {
                m_imageFilename = value;
                NotifyOfPropertyChange();
            }
        }
        private string m_imageFilename;
    }
}
