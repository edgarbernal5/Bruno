
namespace BrunoFramework.Editor.Units
{
    public interface IConsoleService
    {
        void AppendLine(string message);

        void Append(string message);

        void Clear();

        void Flush();
    }
}
