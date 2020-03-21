using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net.Containers;

namespace TrioWpfFramework.Editor.Commands
{
    public class CommandItemCollection : NamedItemCollection<ICommandItem>
    {
        public void Add(params ICommandItem[] commands)
        {
            AddRange(commands);
        }
    }
}
