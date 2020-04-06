using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Framework
{
    public interface IScreenConductor
    {
        IEnumerable<object> Items { get; }

        IEnumerable<object> ActiveItems { get; }

        Task<bool> ActivateItemAsync(object item);

        Task<bool> DeactivateItemAsync(object item, bool close);
    }
}
