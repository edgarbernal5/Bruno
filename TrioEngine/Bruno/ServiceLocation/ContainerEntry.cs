using System;

namespace Bruno.ServiceLocation
{
    internal sealed class ContainerEntry
    {
        public readonly Func<ServiceContainer, object> CreateInstance;

        public readonly DisposalPolicy DisposalPolicy;

        public object InstanceCached;

        public ContainerEntry(Func<ServiceContainer, object> createInstance, DisposalPolicy disposalPolicy)
        {
            CreateInstance = createInstance;
            DisposalPolicy = disposalPolicy;
        }
    }
}
