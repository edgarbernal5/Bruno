using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using TrioApi.Net.Containers;

namespace TrioApi.Net.Graphics.Core
{
    public class EffectPassCollection : NamedList<EffectPass>
    {
        public EffectPassCollection(EffectPass[] passes) : base(passes)
        {

        }
    }
}