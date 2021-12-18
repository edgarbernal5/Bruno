
namespace BrunoFramework.Editor
{
    public abstract class EditorDockableTabViewModel : Screen
    {
        public string Title
        {
            get => DisplayName;
        }

        public string DockId { get; protected set; }
    }
}
