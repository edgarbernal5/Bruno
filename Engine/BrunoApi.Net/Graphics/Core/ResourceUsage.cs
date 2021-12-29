using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BrunoApi.Net.Graphics.Core
{
    public enum ResourceUsage
    {
        //Lectura y escritura en la GPU (ej: Depth buffer)
#if TRIO_DIRECTX
        Default = 0,
#else
        Default,
#endif

        //El contenido del buffer no cambiará luego de crearse.
#if TRIO_DIRECTX
        Immutable = 1,
#else
        Immutable,
#endif

        //A resource that is accessible by both the GPU (read only) and the CPU (write only). A dynamic resource is a good choice for a resource that will be updated by the CPU at least once per frame
#if TRIO_DIRECTX
        Dynamic = 2,
#else
        Dynamic,
#endif

#if TRIO_DIRECTX
        //A resource that supports data transfer (copy) from the GPU to the CPU.
        Staging = 3
#else
		Staging,
#endif
    }
}
