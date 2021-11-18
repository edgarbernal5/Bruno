
using System;

namespace BrunoFramework.Editor.Units
{
    public class ContentBrowserUnit : EditorUnit, IContentBrowserService
    {
        public ContentBrowser ContentBrowser
        {
            get => m_contentBrowserViewModel.Content;
            set => m_contentBrowserViewModel.Content = value;
        }
        private ContentBrowserViewModel m_contentBrowserViewModel;

        public ContentBrowserViewModel ViewModel => m_contentBrowserViewModel;

        public ContentBrowserUnit()
        {
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IContentBrowserService), null, this);
            m_contentBrowserViewModel = new ContentBrowserViewModel(Editor);
        }

        protected override void OnStartup()
        {
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IContentBrowserService));
        }

        protected override void OnShutdown()
        {
            m_contentBrowserViewModel = null;
        }
    }
}
