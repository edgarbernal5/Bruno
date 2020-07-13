using Estero.Collections;

namespace TrioWpfFramework.Net.Graphics.Core
{
    public class EffectPassCollection : FixedSizeNamedList<EffectPass>
    {
        public EffectPassCollection(EffectPass[] passes) 
            : base(passes)
        {

        }
    }
}