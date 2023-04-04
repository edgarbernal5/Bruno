
namespace BrunoFramework.Editor.Units
{
    public interface IContentBrowserService
    {
        ContentBrowser ContentBrowser { get; set; }
        ContentBrowserViewModel ViewModel { get; }

        void Load(string rootDirectory);
        void Search(string keyword);
    }
}
