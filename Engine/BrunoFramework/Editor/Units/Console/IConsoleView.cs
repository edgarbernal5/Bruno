
namespace BrunoFramework.Editor.Units.Console
{
    public interface IConsoleView
    {
        void Clear();
        void AppendText(string text);
        void ScrollToEnd();
        void SetText(string text);
    }
}
