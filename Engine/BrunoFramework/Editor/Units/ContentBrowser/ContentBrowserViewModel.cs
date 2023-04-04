
using System.IO;
using System.Windows.Input;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserViewModel : EditorDockTabViewModel
    {
        internal const string DockIdString = "ContentBrowser";

        public ContentBrowser Content
        {
            get => m_content;
            set
            {
                Set(ref m_content, value);
            }
        }
        private ContentBrowser m_content;

        public string FindInputText
        {
            get => m_findInputText;
            set
            {
                Set(ref m_findInputText, value);
            }
        }
        private string m_findInputText;

        public ICommand DoubleClickCommand
        {
            get
            {
                if (m_doubleClickCommand == null)
                {
                    m_doubleClickCommand = new DelegateCommand<ContentBrowserItem>(item =>
                      {
                          if (item == null) return;

                          if (item.IsFolder)
                          {
                              OnDoubleClickFolderItem(item.RelativePath);
                              return;
                          }
                          OnDoubleClickFileItem(item);
                      });
                }
                return m_doubleClickCommand;
            }
        }
        private ICommand m_doubleClickCommand;
        public ICommand GoBackCommand
        {
            get
            {
                if (m_goBackCommand == null)
                {
                    m_goBackCommand = new DelegateCommand(() =>
                      {
                          OnDoubleClickFolderItem(Path.GetDirectoryName(m_content.CurrentPath));
                      }, () => m_content != null && !string.IsNullOrEmpty(m_content.CurrentPath));
                }
                return m_goBackCommand;
            }
        }
        private DelegateCommand m_goBackCommand;

        //https://www.codeproject.com/Articles/1206685/Advanced-WPF-TreeViews-Part-of-n
        //public ICommand ExpandTreeFolderCommand
        //{
        //    get
        //    {
        //        if (m_expandTreeFolderCommand == null)
        //        {
        //            m_expandTreeFolderCommand = new DelegateCommand<object>((x) =>
        //            {
        //                int aa = 33;
        //            });
        //        }
        //        return m_expandTreeFolderCommand;
        //    }
        //}
        //private ICommand m_expandTreeFolderCommand;

        public ICommand FindContentCommand
        {
            get
            {
                if (m_findContentCommand == null)
                {
                    m_findContentCommand = new DelegateCommand(() =>
                    {
                        m_contentBrowserService.Search(FindInputText);
                    });
                }
                return m_findContentCommand;
            }
        }
        private ICommand m_findContentCommand;

        public FilterByType FilterByType
        {
            get => m_filterByType;
            set
            {
                if (Set(ref m_filterByType, value))
                {
                    Content.OnFilterByType(m_filterByType);
                }
            }
        }
        private FilterByType m_filterByType = FilterByType.All;

        private IContentBrowserService m_contentBrowserService;

        public ContentBrowserViewModel(IEditorService editor)
            : base()
        {
            DisplayName = "Content Browser";
            DockId = DockIdString;

            m_contentBrowserService = editor.Services.GetInstance<IContentBrowserService>();
        }

        private void OnDoubleClickFileItem(ContentBrowserItem item)
        {
            Content.OnDoubleClickFileItem(item);
        }
        
        private void OnDoubleClickFolderItem(string path)
        {
            Content.OnDoubleClickFolderItem(path);
            m_goBackCommand.RaiseCanExecuteChanged();
        }
    }

    public enum FilterByType
    {
        All,
        Scenes,
        Textures,
    }
}