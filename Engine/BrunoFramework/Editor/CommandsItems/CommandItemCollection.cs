using Bruno.Collections;

namespace BrunoFramework.Editor
{
    public class CommandItemCollection : NamedItemCollection<ICommandItem>
    {
        public void Add(params ICommandItem[] commands)
        {
            AddRange(commands);
        }
    }
}
