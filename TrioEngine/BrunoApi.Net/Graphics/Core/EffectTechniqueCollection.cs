using Bruno.Collections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoApi.Net.Graphics.Core
{
    public class EffectTechniqueCollection : FixedSizeNamedList<EffectTechnique>
    {
        public EffectTechniqueCollection(EffectTechnique[] techniques) 
            : base(techniques)
        {

        }
    }
}
