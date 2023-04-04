
using System.Windows.Input;

namespace BrunoFramework.Editor
{
    public interface IEditorDockTab : IScreen
    {
        ICommand CloseCommand { get; }
        bool IsSelected { get; set; }
    }
}
