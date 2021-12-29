using Bruno.Interop;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace Bruno.ServiceLocation
{
    public class ServiceContainer : DisposableBase
    {
        private static readonly Type delegateType = typeof(Delegate);
        private static readonly Type enumerableType = typeof(IEnumerable);
        private readonly Dictionary<ContainerRegistration, ContainerEntry> _registrations;

        public bool EnablePropertyInjection { get; set; }

        public ServiceContainer()
        {
            _registrations = new Dictionary<ContainerRegistration, ContainerEntry>();

            RegisterInstance(typeof(ServiceContainer), null, this);
        }

        public void Clear()
        {
            lock (_registrations)
            {
                foreach (var entry in _registrations.Values)
                    DisposeInstances(entry);

                _registrations.Clear();
            }
        }

        public void RegisterInstance(Type service, string key, object implementation)
        {
            RegisterHandler(service, key, container => implementation, DisposalPolicy.Manual);
        }

        public void RegisterPerRequest(Type service, string key, Type implementation)
        {
            RegisterHandler(service, key, container => container.BuildInstance(implementation), DisposalPolicy.Automatic);
        }

        public void RegisterSingleton(Type service, string key, Type implementation)
        {
            object singleton = null;
            RegisterHandler(service, key, container => singleton ?? (singleton = container.BuildInstance(implementation)), DisposalPolicy.Automatic);
        }

        public void RegisterHandler(Type service, string key, Func<ServiceContainer, object> createInstance, DisposalPolicy disposalPolicy = DisposalPolicy.Manual)
        {
            var registration = new ContainerRegistration(service, key);
            var entry = new ContainerEntry(createInstance, disposalPolicy);
            lock (_registrations)
            {
                _registrations[registration] = entry;
            }
        }

        public void UnregisterHandler(Type service, string key)
        {
            var registration = new ContainerRegistration(service, key);
            lock (_registrations)
            {
                if (_registrations.TryGetValue(registration, out ContainerEntry entry))
                {
                    DisposeInstances(entry);
                    _registrations.Remove(registration);
                }
            }
        }

        public void UnregisterHandler(Type service)
        {
            lock (_registrations)
            {
                var entries = _registrations.Where(registry => registry.Key.Type == service).ToArray();
                foreach (var entry in entries)
                {
                    DisposeInstances(entry.Value);
                    _registrations.Remove(entry.Key);
                }
            }
        }

        public TService GetInstance<TService>()
        {
            return (TService)GetInstance(typeof(TService), null);
        }

        public object GetInstance(Type service, string key)
        {
            var entry = GetEntry(service, key);
            if (entry != null)
            {
                var instance = entry.CreateInstance(this);

                if (EnablePropertyInjection && instance != null)
                {
                    BuildUp(instance);
                }

                entry.InstanceCached = instance;
                return instance;
            }

            if (service == null)
            {
                return null;
            }

            if (delegateType.GetTypeInfo().IsAssignableFrom(service.GetTypeInfo()))
            {
                var typeToCreate = service.GetTypeInfo().GenericTypeArguments[0];
                var factoryFactoryType = typeof(FactoryFactory<>).MakeGenericType(typeToCreate);
                var factoryFactoryHost = Activator.CreateInstance(factoryFactoryType);
                var factoryFactoryMethod = factoryFactoryType.GetRuntimeMethod("Create", new Type[] { typeof(ServiceContainer) });
                return factoryFactoryMethod.Invoke(factoryFactoryHost, new object[] { this });
            }

            if (enumerableType.GetTypeInfo().IsAssignableFrom(service.GetTypeInfo()) && service.GetTypeInfo().IsGenericType)
            {
                var listType = service.GetTypeInfo().GenericTypeArguments[0];
                var instances = GetAllInstances(listType).ToList();
                var array = Array.CreateInstance(listType, instances.Count);

                for (var i = 0; i < array.Length; i++)
                {
                    if (EnablePropertyInjection)
                    {
                        BuildUp(instances[i]);
                    }

                    array.SetValue(instances[i], i);
                }

                return array;
            }

            return null;
        }

        public bool HasHandler(Type service, string key)
        {
            return GetEntry(service, key) != null;
        }

        private IEnumerable<ContainerRegistration> GetRegistrations(Type service)
        {
            IEnumerable<ContainerRegistration> registrations;

            lock (_registrations)
            {
                registrations = _registrations.Keys.Where(r => r.Type == service).ToArray();
            }
            return registrations;
        }

        public IEnumerable<object> GetAllInstances(Type service, string key = null)
        {
            var registrations = GetRegistrations(service).Distinct();

            var instances = new List<object>();
            foreach (var registration in registrations)
            {
                if (registration.Key != null)  // Only named instances.
                {
                    var instance = GetInstance(service, registration.Key);
                    if (instance != null)
                        instances.Add(instance);
                }
            }

            return instances;
        }

        public void BuildUp(object instance)
        {
            var properties = instance
                .GetType()
                .GetRuntimeProperties()
                .Where(p => p.CanRead && p.CanWrite && p.PropertyType.GetTypeInfo().IsInterface);

            foreach (var property in properties)
            {
                var value = GetInstance(property.PropertyType, null);

                if (value != null)
                {
                    property.SetValue(instance, value, null);
                }
            }
        }

        private ContainerEntry GetEntry(Type service, string key)
        {
            ContainerEntry entry;
            var registration = new ContainerRegistration(service, key);
            if (_registrations.TryGetValue(registration, out entry))
            {
                return entry;
            }
            return null;
        }

        protected object BuildInstance(Type type)
        {
            var args = DetermineConstructorArgs(type);
            return ActivateInstance(type, args);
        }

        protected virtual object ActivateInstance(Type type, object[] args)
        {
            var instance = args.Length > 0 ? 
                System.Activator.CreateInstance(type, args) : 
                System.Activator.CreateInstance(type);

            Activated(instance);
            return instance;
        }

        public event Action<object> Activated = delegate { };

        private object[] DetermineConstructorArgs(Type implementation)
        {
            var args = new List<object>();
            var constructor = SelectEligibleConstructor(implementation);

            if (constructor != null)
            {
                args.AddRange(constructor.GetParameters().Select(info => GetInstance(info.ParameterType, null)));
            }

            return args.ToArray();
        }

        private ConstructorInfo SelectEligibleConstructor(Type type)
        {
            return type.GetTypeInfo().DeclaredConstructors
                .Where(c => c.IsPublic)
                .Select(c => new
                {
                    Constructor = c,
                    HandledParamters = c.GetParameters().Count(p => HasHandler(p.ParameterType, null))
                })
                .OrderByDescending(c => c.HandledParamters)
                .Select(c => c.Constructor)
                .FirstOrDefault();
        }

        protected override void OnDisposing(bool disposing)
        {
            if (disposing)
            {
                lock (_registrations)
                {
                    foreach (var entry in _registrations.Values)
                    {
                        DisposeInstances(entry);
                    }
                    _registrations.Clear();
                }
            }
        }

        private void DisposeInstances(ContainerEntry entry)
        {
            if (entry.DisposalPolicy != DisposalPolicy.Automatic)
                return;

            if (entry.InstanceCached is IDisposable)
            {
                var disposable = entry.InstanceCached as IDisposable;
                disposable.Dispose();
            }
        }
    }

}
