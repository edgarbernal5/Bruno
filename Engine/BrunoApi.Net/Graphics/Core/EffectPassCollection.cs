using Bruno.Collections;

namespace BrunoApi.Net.Graphics.Core
{
    public class EffectPassCollection : FixedSizeNamedList<EffectPass>
    {
        public EffectPassCollection(EffectPass[] passes) 
            : base(passes)
        {

        }
    }
}