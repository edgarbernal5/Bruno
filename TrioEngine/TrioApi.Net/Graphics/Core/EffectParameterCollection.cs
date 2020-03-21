using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioWpfFramework.Net.Containers;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class EffectParameterCollection : FixedSizeNamedList<EffectParameter>
    {
        public EffectParameterCollection(EffectParameter[] parameters) : base(parameters)
        {

        }
    }
}
