
using System;
using System.IO;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserPopulate
    {
        private readonly ContentBrowser m_contentBrowmser;
        private readonly IStorage m_storage;

        public ContentBrowserPopulate(ContentBrowser contentBrowser, IStorage storage)
        {
            m_contentBrowmser = contentBrowser;
            m_storage = storage;
        }

        public void Execute()
        {
            m_contentBrowmser.Clear();

            foreach (var item in m_storage.GetDirectories(""))
            {
                m_contentBrowmser.FileListItems.Items.Add
                    (
                        new ContentBrowserItem()
                        {
                            Text = Path.GetFileName(item),
                            Icon = GetIconFor("folder")
                        }
                    );
            }

            foreach (var item in m_storage.GetFiles(""))
            {
                m_contentBrowmser.FileListItems.Items.Add
                    (
                        new ContentBrowserItem()
                        {
                            Text = Path.GetFileName(item),
                            Icon = GetIconFor("file")
                        }
                    );
            }
        }

        private ImageSource GetIconFor(string filename)
        {
            var yourImage = new BitmapImage(new Uri(string.Format("Resources/Icons/{0}.jpg", filename), UriKind.Relative));
            yourImage.Freeze(); // -> to prevent error: "Must create DependencySource on same Thread as the DependencyObject"
            
            return yourImage;
        }
    }
}
