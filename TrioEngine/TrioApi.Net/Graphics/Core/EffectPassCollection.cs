using Estero.Collections;

namespace TrioApi.Net.Graphics.Core
{
    public class EffectPassCollection : FixedSizeNamedList<EffectPass>
    {
        public EffectPassCollection(EffectPass[] passes) 
            : base(passes)
        {

        }
    }
}