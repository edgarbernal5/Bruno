using Estero.Collections;

namespace EsteroFramework.Editor
{
    public class CommandItemCollection : NamedItemCollection<ICommandItem>
    {
        public void Add(params ICommandItem[] commands)
        {
            AddRange(commands);
        }
    }
}
