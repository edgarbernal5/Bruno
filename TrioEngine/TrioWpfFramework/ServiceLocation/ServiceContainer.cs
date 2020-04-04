using CommonServiceLocator;
using Estero.Collections;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Estero;

namespace TrioWpfFramework.ServiceLocation
{
    public partial class ServiceContainer : IDisposable
    {
        private readonly Dictionary<ServiceRegistration, ServiceEntry> _registry;


        /// <summary>
        /// Gets a value indicating whether this instance has been disposed of.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if this instance has been disposed of; otherwise,
        /// <see langword="false"/>.
        /// </value>
        public bool IsDisposed { get; private set; }


        /// <summary>
        /// Initializes a new instance of the <see cref="ServiceContainer"/> class.
        /// </summary>
        public ServiceContainer()
        {
            _registry = new Dictionary<ServiceRegistration, ServiceEntry>();

            // Register default services. (Needed for automatic parameter injection.)
            Register(typeof(IServiceProvider), null, container => container, CreationPolicy.LocalShared, DisposalPolicy.Manual);
            Register(typeof(IServiceLocator), null, container => container, CreationPolicy.LocalShared, DisposalPolicy.Manual);
            Register(typeof(ServiceContainer), null, container => container, CreationPolicy.LocalShared, DisposalPolicy.Manual);
        }


        /// <overloads>
        /// <summary>
        /// Releases all resources used by an instance of the <see cref="ServiceContainer"/> class.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Releases all resources used by an instance of the <see cref="ServiceContainer"/> class.
        /// </summary>
        /// <remarks>
        /// This method calls the virtual <see cref="Dispose(bool)"/> method, passing in 
        /// <see langword="true"/>, and then suppresses finalization of the instance.
        /// </remarks>
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }


        /// <summary>
        /// Releases the unmanaged resources used by an instance of the
        /// <see cref="ServiceContainer"/> class and optionally releases the managed resources.
        /// </summary>
        /// <param name="disposing">
        /// <see langword="true"/> to release both managed and unmanaged resources;
        /// <see langword="false"/> to release only unmanaged resources.
        /// </param>
        protected virtual void Dispose(bool disposing)
        {
            if (!IsDisposed)
            {
                if (disposing)
                {
                    // Dispose tracked service instances.
                    lock (_registry)
                    {
                        foreach (var entry in _registry.Values)
                            DisposeInstances(entry);
                    }

                }

                IsDisposed = true;
            }
        }

        /// <summary>
        /// Throws an <see cref="ObjectDisposedException"/> if the service container has already
        /// been disposed.
        /// </summary>
        private void ThrowIfDisposed()
        {
            if (IsDisposed)
                throw new ObjectDisposedException(GetType().FullName);
        }


        /// <summary>
        /// Stores the shared service instance, or keeps track of disposable instances for automatic
        /// disposal.
        /// </summary>
        /// <param name="entry">The service entry in the registry.</param>
        /// <param name="instance">The service instance. (Can be <see langword="null"/>.)</param>
        private static void StoreInstance(ServiceEntry entry, object instance)
        {
            if (instance == null)
                return;

            if (entry.CreationPolicy == CreationPolicy.Shared || entry.CreationPolicy == CreationPolicy.LocalShared)
            {
                // Store shared instance.
                Debug.Assert(entry.Instances == null);
                entry.Instances = instance;
            }
            else
            {
                // Keep track of non-shared instance for automatic disposal.
                Debug.Assert(entry.CreationPolicy == CreationPolicy.NonShared);
                if (entry.DisposalPolicy == DisposalPolicy.Automatic)
                {
                    var disposable = instance as IDisposable;
                    if (disposable != null)
                    {
                        if (entry.Instances == null)
                        {
                            // This the first service instance.
                            entry.Instances = new WeakReference(disposable);
                        }
                        else if (entry.Instances is WeakReference)
                        {
                            // This is the second service instance.
                            var weakReference = (WeakReference)entry.Instances;
                            var previousDisposable = (IDisposable)weakReference.Target;
                            if (previousDisposable == null)
                            {
                                // First service instance has already been garbage collected.
                                weakReference.Target = instance;
                            }
                            else
                            {
                                // First service instance is still alive.
                                entry.Instances = new WeakCollection<IDisposable>
                                {
                                    previousDisposable,
                                    disposable
                                };
                            }
                        }
                        else if (entry.Instances is WeakCollection<IDisposable>)
                        {
                            // Multiple service instance.
                            var weakCollection = (WeakCollection<IDisposable>)entry.Instances;
                            weakCollection.Add(disposable);
                        }
                    }
                }
            }
        }


        /// <summary>
        /// Disposes of objects whose lifetime is controlled by the service container.
        /// </summary>
        /// <param name="entry">The service entry in the registry.</param>
        private static void DisposeInstances(ServiceEntry entry)
        {
            if (entry.DisposalPolicy != DisposalPolicy.Automatic)
                return;

            if (entry.Instances is IDisposable)
            {
                // Shared disposable instance.
                Debug.Assert(entry.CreationPolicy == CreationPolicy.Shared || entry.CreationPolicy == CreationPolicy.LocalShared);
                var instance = (IDisposable)entry.Instances;
                instance.Dispose();
            }
            else if (entry.Instances is WeakReference)
            {
                // Single non-shared disposable instance.
                Debug.Assert(entry.CreationPolicy == CreationPolicy.NonShared);
                var instance = ((WeakReference)entry.Instances).Target;
                instance.SafeDispose();
            }
            else if (entry.Instances is WeakCollection<IDisposable>)
            {
                //  Multiple non-shared disposable instance.
                Debug.Assert(entry.CreationPolicy == CreationPolicy.NonShared);
                var instances = (WeakCollection<IDisposable>)entry.Instances;
                foreach (var instance in instances)
                    instance.Dispose();
            }
        }


        /// <summary>
        /// Gets all service registrations for the current container up to the root container.
        /// </summary>
        /// <returns>The service registrations. (May include duplicate items.)</returns>
        private IEnumerable<ServiceRegistration> GetRegistrations()
        {
            IEnumerable<ServiceRegistration> registrations;

            lock (_registry)
            {
                // Copy registrations into buffer (to release lock as soon as possible).
                registrations = _registry.Keys.ToArray();
            }

            return registrations;
        }


        /// <summary>
        /// Gets all registrations of a given service type for the current container up to the root
        /// container.
        /// </summary>
        /// <returns>
        /// The registration of the given service type. (May include duplicate items.)
        /// </returns>
        private IEnumerable<ServiceRegistration> GetRegistrations(Type serviceType)
        {
            IEnumerable<ServiceRegistration> registrations;

            lock (_registry)
            {
                // Copy registrations into buffer (to release lock as soon as possible).
                registrations = _registry.Keys.Where(r => r.Type == serviceType).ToArray();
            }

            return registrations;
        }


        /// <summary>
        /// Resets the container and removes all locally registered service types.
        /// </summary>
        public void Clear()
        {
            ThrowIfDisposed();

            lock (_registry)
            {
                // Dispose tracked service instances.
                foreach (var entry in _registry.Values)
                    DisposeInstances(entry);

                _registry.Clear();
            }
        }


        /// <overloads>
        /// <summary>
        /// Registers a service.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Registers a service using a custom factory method and certain creation and disposal
        /// policies.
        /// </summary>
        /// <param name="serviceType">The type of the service.</param>
        /// <param name="key">
        /// The name under which the object should be registered. Can be <see langword="null"/> or
        /// empty.
        /// </param>
        /// <param name="createInstance">
        /// The factory method responsible for serving the requests from the container.
        /// </param>
        /// <param name="creationPolicy">
        /// The creation policy that specifies when and how a service will be instantiated.
        /// </param>
        /// <param name="disposalPolicy">
        /// The disposal policy that specifies when a service instance will be disposed. (Only
        /// relevant if the service instance implements <see cref="IDisposable"/>.)
        /// </param>
        /// <remarks>
        /// If a service with the same type and name is already registered, the existing entry will
        /// be replaced.
        /// </remarks>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="serviceType"/> or <paramref name="createInstance"/> is
        /// <see langword="null"/>.
        /// </exception>
        public void Register(Type serviceType, string key, Func<ServiceContainer, object> createInstance, CreationPolicy creationPolicy, DisposalPolicy disposalPolicy)
        {
            ThrowIfDisposed();

            if (serviceType == null)
                throw new ArgumentNullException(nameof(serviceType));
            if (createInstance == null)
                throw new ArgumentNullException(nameof(createInstance));

            var registration = new ServiceRegistration(serviceType, key);
            var entry = new ServiceEntry(createInstance, creationPolicy, disposalPolicy);
            lock (_registry)
            {
                _registry[registration] = entry;
            }
        }


        /// <overloads>
        /// <summary>
        /// Unregisters a service.
        /// </summary>
        /// </overloads>
        /// 
        /// <summary>
        /// Unregisters all services of the given service type.
        /// </summary>
        /// <param name="serviceType">The type of service to be removed.</param>
        /// <remarks>
        /// The method removes all services (named and unnamed) that match the given type.
        /// </remarks>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="serviceType"/> is <see langword="null"/>.
        /// </exception>
        public void Unregister(Type serviceType)
        {
            ThrowIfDisposed();

            if (serviceType == null)
                throw new ArgumentNullException(nameof(serviceType));

            lock (_registry)
            {
                var services = _registry.Where(service => service.Key.Type == serviceType).ToArray();
                foreach (var service in services)
                {
                    DisposeInstances(service.Value);
                    _registry.Remove(service.Key);
                }
            }
        }


        /// <summary>
        /// Unregisters the service with the specified name.
        /// </summary>
        /// <param name="serviceType">The type of service to be removed.</param>
        /// <param name="key">
        /// The name the object was registered with. Can be <see langword="null"/> or empty. 
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="serviceType"/> is <see langword="null"/>.
        /// </exception>
        public void Unregister(Type serviceType, string key)
        {
            ThrowIfDisposed();

            if (serviceType == null)
                throw new ArgumentNullException(nameof(serviceType));

            var registration = new ServiceRegistration(serviceType, key);
            lock (_registry)
            {
                ServiceEntry entry;
                if (_registry.TryGetValue(registration, out entry))
                {
                    DisposeInstances(entry);
                    _registry.Remove(registration);
                }
            }
        }


        private IEnumerable<object> GetAllInstancesImpl(Type serviceType)
        {
            // Get the registrations for the given type from the current container up 
            // to the root container. (GetRegistrations() may return duplicate entries. 
            // Use Enumerable.Distinct() to keep only the first registration for any 
            // given name.)
            var registrations = GetRegistrations(serviceType).Distinct();

            // Try to get all instances.
            var instances = new List<object>();
            foreach (var registration in registrations)
            {
                if (registration.Key != null)  // Only named instances.
                {
                    var instance = GetInstanceImpl(serviceType, registration.Key);
                    if (instance != null)
                        instances.Add(instance);
                }
            }

            return instances;
        }


        private object GetInstanceImpl(Type serviceType, string key, bool onlyShared = false)
        {
            lock (_registry)
            {
                // Try to locate matching service entry in the current or a parent container.
                ServiceRegistration registration = new ServiceRegistration(serviceType, key);
                ServiceEntry entry;
                ServiceContainer container;

                if (_registry.TryGetValue(registration, out entry))
                {
                    // Service entry found in local registry.
                    container = this;
                }

                if (entry != null)
                {
                    // Resolve instance from service entry.
                    if (entry.CreationPolicy == CreationPolicy.NonShared)
                    {
                        if (onlyShared)
                            return null;

                        // Create non-shared (transient) service instance.
                        var instance = entry.CreateInstance(this);

                        // Track for disposal, if necessary.
                        StoreInstance(entry, instance);

                        return instance;
                    }

                    //if (entry.CreationPolicy == CreationPolicy.LocalShared && container != this)
                    //{
                    //    // Service entry was found in a parent container, but the service is 
                    //    // configured to be created per container. --> Copy into local registry.
                    //    entry = new ServiceEntry(entry);
                    //    _registry.Add(registration, entry);
                    //    container = this;
                    //}

                    // Create shared service instance, if not already cached.
                    // (Double-check to avoid unnecessary lock.)
                    if (entry.Instances == null)
                    {
                        lock (entry)
                        {
                            if (entry.Instances == null)
                            {
                                //var instance = entry.CreateInstance(container);
                                //StoreInstance(entry, instance);
                            }
                        }
                    }

                    return entry.Instances;
                }
            }

            // The requested service type is not directly registered. 

            // Other supported types are:  
            // - IEnumerable<TService> ... service instance collection
            // - Func<TService> .......... factory method for lazy resolution

            if (serviceType.IsGenericType)
            {
                var genericType = serviceType.GetGenericTypeDefinition();
                if (genericType == typeof(IEnumerable<>))
                {
                    // Requested type is IEnumerable<TService>.

                    // Get typeof(TService).
                    Type actualServiceType = serviceType.GetGenericArguments()[0];

                    // Get array of all named TService instances.
                    object[] instances = GetAllInstancesImpl(actualServiceType).ToArray();

                    // Create and fill TService[] array.
                    Array array = Array.CreateInstance(actualServiceType, instances.Length);
                    for (int i = 0; i < array.Length; i++)
                        array.SetValue(instances[i], i);

                    return array;
                }

                if (genericType == typeof(Func<>))
                {
                    // Requested type is Func<TService>.
                    var actualServiceType = serviceType.GetGenericArguments()[0];
                    var factoryFactoryType = typeof(ServiceFactory<>).MakeGenericType(actualServiceType);
                    var factoryFactoryInstance = Activator.CreateInstance(factoryFactoryType);
                    var factoryFactoryMethod = factoryFactoryType.GetMethod("Create");

                    return factoryFactoryMethod.Invoke(factoryFactoryInstance, new object[] { this, key });
                }

                //if (genericType == typeof(Func<,>) && serviceType.GetGenericArguments()[0] == typeof(string))
                //{
                //  // Requested type is Func<string, TService> where the argument is the name of the service.
                //  ...
                //}
            }

            return null;
        }


        /// <summary>
        /// Creates an instance the given type and satisfies the constructor dependencies.
        /// </summary>
        /// <param name="type">The type to instantiate.</param>
        /// <returns>A new instance of the requested type.</returns>
        ///// <remarks>
        ///// <strong>Notes to Inheritors:</strong> The method can be overridden in derived classes to use
        ///// a different strategy for creating a new instance of the requested type. The base 
        ///// implementation calls <see cref="SelectConstructor"/> to select the appropriate constructor,
        ///// resolves the constructor arguments and then calls <see cref="OnCreateInstance"/> to activate
        ///// the instance using reflection.
        ///// </remarks>
        public object CreateInstance(Type type)
        {
            var args = ResolveConstructorArgs(type);
            var instance = OnCreateInstance(type, args);
            return instance;
        }


        private object[] ResolveConstructorArgs(Type type)
        {
            // Select the constructor.
            var constructor = SelectConstructor(type);

            // Resolve the constructor's dependencies.
            var args = new List<object>();
            if (constructor != null)
            {
                foreach (var parameter in constructor.GetParameters())
                {
                    var value = GetInstance(parameter.ParameterType, null);
                    args.Add(value);
                }
            }

            return args.ToArray();
        }


        /// <summary>
        /// Selects the constructor to be used for activating the given type.
        /// </summary>
        /// <param name="type">The type to be activated.</param>
        /// <returns>The constructor that should be used.</returns>
        /// <remarks>
        /// <strong>Notes to Inheritors:</strong> This method can be overridden in derived classes
        /// if a certain strategy for choosing the constructor should be applied. The base
        /// implementation chooses the constructor with the max number of parameters.
        /// </remarks>
        protected virtual ConstructorInfo SelectConstructor(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            ConstructorInfo preferredConstructor = null;
            int preferredNumberOfParameters = -1;
            foreach (var constructor in type.GetConstructors())
            {
                int numberOfParameters = constructor.GetParameters().Length;

                if (numberOfParameters > preferredNumberOfParameters)
                {
                    preferredConstructor = constructor;
                    preferredNumberOfParameters = numberOfParameters;
                }
            }

            return preferredConstructor;
        }


        /// <summary>
        /// Creates an instance of the type with the specified constructor arguments.
        /// </summary>
        /// <param name="type">The type of the instance.</param>
        /// <param name="args">The constructor arguments.</param>
        /// <returns>A new instance of the requested type.</returns>
        /// <remarks>
        /// <strong>Notes to Inheritors:</strong> When overriding <see cref="OnCreateInstance"/> be
        /// sure to throw an exception if the activation fails. The method must not return
        /// <see langword="null"/>.
        /// </remarks>
        protected virtual object OnCreateInstance(Type type, object[] args)
        {
            return (args != null && args.Length > 0)
                   ? Activator.CreateInstance(type, args)
                   : Activator.CreateInstance(type);
        }


        /// <summary>
        /// Tries to resolve all property dependencies of the given instance.
        /// </summary>
        /// <param name="instance">The instance to build up.</param>
        /// <remarks>
        /// This method inspects the properties of the given <paramref name="instance"/>. It will
        /// initialize the property with an instance from this container if the property has a
        /// public setter, if the property type is found in the container, and if the property type
        /// is a reference type (not a value type).
        /// </remarks>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="instance"/> is <see langword="null"/>.
        /// </exception>
        /// <exception cref="ActivationException">
        /// An error occurred while resolving a service instance.
        /// </exception>
        public void ResolveProperties(object instance)
        {
            ThrowIfDisposed();

            if (instance == null)
                throw new ArgumentNullException(nameof(instance));

            var instanceType = instance.GetType();

            foreach (var property in instanceType.GetProperties())
            {
                // Check whether property has public getter and setter.
                if (property.GetSetMethod() == null || property.GetGetMethod() == null)
                    continue;

                // Get the type of the property (= the service type to resolve).
                var propertyType = property.PropertyType;

                // Ignore value types.
                if (propertyType.IsValueType)
                    continue;

                // Ignore property if a value is already set.
                if (property.GetValue(instance, null) != null)
                    continue;

                // Try to resolve and inject dependency.
                var injection = GetInstance(propertyType, null);
                if (injection != null)
                    property.SetValue(instance, injection, null);
            }
        }
        
    }
}