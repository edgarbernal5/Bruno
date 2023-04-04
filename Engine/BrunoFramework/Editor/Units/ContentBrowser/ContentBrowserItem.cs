
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

        public bool IsFolder
        {
            get => m_isFolder;
            set
            {
                m_isFolder = value;
            }
        }
        private bool m_isFolder;

        public string RelativePath
        {
            get => m_relativePath;
            set
            {
                m_relativePath = value;
            }
        }
        private string m_relativePath;

        public string AbsolutePath
        {
            get => m_absolutePath;
            set
            {
                m_absolutePath = value;
            }
        }
        private string m_absolutePath;
    }
}
