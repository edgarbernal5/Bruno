using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

namespace Estero.ServiceLocation
{
    /// <summary>
    /// A simple IoC container.
    /// </summary>
    public class ServiceContainer
    {
        private static readonly Type delegateType = typeof(Delegate);
        private static readonly Type enumerableType = typeof(IEnumerable);
        private readonly List<ContainerEntry> entries;

        /// <summary>
        /// Whether to enable recursive property injection for all resolutions.
        /// </summary>
        public bool EnablePropertyInjection { get; set; }

        public ServiceContainer()
        {
            entries = new List<ContainerEntry>();

            RegisterInstance(typeof(ServiceContainer), null, this);
        }

        public void RegisterInstance(Type service, string key, object implementation)
        {
            RegisterHandler(service, key, container => implementation);
        }

        public void RegisterPerRequest(Type service, string key, Type implementation)
        {
            RegisterHandler(service, key, container => container.BuildInstance(implementation));
        }

        public void RegisterSingleton(Type service, string key, Type implementation)
        {
            object singleton = null;
            RegisterHandler(service, key, container => singleton ?? (singleton = container.BuildInstance(implementation)));
        }

        public void RegisterHandler(Type service, string key, Func<ServiceContainer, object> handler)
        {
            GetOrCreateEntry(service, key).Add(handler);
        }

        public void UnregisterHandler(Type service, string key)
        {
            var entry = GetEntry(service, key);
            if (entry != null)
            {
                entries.Remove(entry);
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
                var instance = entry.Single()(this);

                if (EnablePropertyInjection && instance != null)
                {
                    BuildUp(instance);
                }

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

        public IEnumerable<object> GetAllInstances(Type service, string key = null)
        {
            var entries = GetEntry(service, key);

            if (entries == null)
            {
                return new object[0];
            }

            var instances = entries.Select(e => e(this));

            foreach (var instance in instances)
            {
                if (EnablePropertyInjection && instance != null)
                {
                    BuildUp(instance);
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

        private ContainerEntry GetOrCreateEntry(Type service, string key)
        {
            var entry = GetEntry(service, key);
            if (entry == null)
            {
                entry = new ContainerEntry { Service = service, Key = key };
                entries.Add(entry);
            }

            return entry;
        }

        private ContainerEntry GetEntry(Type service, string key)
        {
            if (service == null)
            {
                return entries.FirstOrDefault(x => x.Key == key);
            }

            if (key == null)
            {
                return entries.FirstOrDefault(x => x.Service == service && x.Key == null)
                       ?? entries.FirstOrDefault(x => x.Service == service);
            }

            return entries.FirstOrDefault(x => x.Service == service && x.Key == key);
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

        private class ContainerEntry : List<Func<ServiceContainer, object>>
        {
            public string Key;
            public Type Service;
        }

        private class FactoryFactory<T>
        {
            public Func<T> Create(ServiceContainer container)
            {
                return () => (T)container.GetInstance(typeof(T), null);
            }
        }
    }

}
