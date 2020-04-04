using Estero.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

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
