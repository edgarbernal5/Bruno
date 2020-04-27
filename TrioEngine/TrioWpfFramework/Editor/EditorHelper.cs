using System;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Windows;
using TrioWpfFramework.ServiceLocation;
using static System.FormattableString;

namespace TrioWpfFramework.Editor
{
    public static class EditorHelper
    {
        /// <summary>
        /// Registers a view for a view model.
        /// </summary>
        /// <param name="serviceContainer">The service container.</param>
        /// <param name="viewModelType">The type of the view model.</param>
        /// <param name="view">The type of the view.</param>
        /// <param name="creationPolicy">
        /// The creation policy that specifies when and how the view will be instantiated.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="serviceContainer"/> or <paramref name="viewModelType"/> is
        /// <see langword="null"/>.
        /// </exception>
        public static void RegisterView(this ServiceContainer serviceContainer, Type viewModelType, Type view, CreationPolicy creationPolicy = CreationPolicy.NonShared)
        {
            if (serviceContainer == null)
                throw new ArgumentNullException(nameof(serviceContainer));
            if (viewModelType == null)
                throw new ArgumentNullException(nameof(viewModelType));

            serviceContainer.Register(typeof(FrameworkElement), viewModelType.FullName, view, creationPolicy);
        }

        /// <summary>
        /// Unregisters a view for a view model.
        /// </summary>
        /// <param name="serviceContainer">The service container.</param>
        /// <param name="viewModelType">The type of the view model.</param>
        /// <exception cref="ArgumentNullException">
        /// <paramref name="serviceContainer"/> or <paramref name="viewModelType"/> is
        /// <see langword="null"/>.
        /// </exception>
        public static void UnregisterView(this ServiceContainer serviceContainer, Type viewModelType)
        {
            if (serviceContainer == null)
                throw new ArgumentNullException(nameof(serviceContainer));
            if (viewModelType == null)
                throw new ArgumentNullException(nameof(viewModelType));

            serviceContainer.Unregister(typeof(FrameworkElement), viewModelType.FullName);
        }

        /// <summary>
        /// Gets the name of the application from the assembly information of the entry assembly.
        /// </summary>
        /// <returns>The name of the application.</returns>
        internal static string GetDefaultApplicationName()
        {
            string applicationName;

            // Try to get the name from the 'Title' attribute of the entry assembly.
            var entryAssembly = Assembly.GetEntryAssembly();
            if (entryAssembly != null)  // Can be null at design-time.
            {
                var titleAttribute = entryAssembly.GetCustomAttributes(typeof(AssemblyTitleAttribute), false)
                                                  .Cast<AssemblyTitleAttribute>()
                                                  .FirstOrDefault();
                applicationName = titleAttribute?.Title;
                if (!string.IsNullOrEmpty(applicationName))
                    return applicationName;
            }

            // Get the name of the entry assembly.
            if (entryAssembly != null)
            {
                applicationName = entryAssembly.GetName().Name;
                if (!string.IsNullOrEmpty(applicationName))
                    return applicationName;
            }

            return "Unnamed Application";
        }


        /// <summary>
        /// Removes the '_' which is used to indicate access keys from the specified text.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <returns>The text without '_'.</returns>
        internal static string FilterAccessKeys(string text)
        {
            if (string.IsNullOrEmpty(text))
                return text;

            int index = text.IndexOf('_');
            if (index >= 0)
                text = text.Remove(index, 1);

            return text;
        }

        /// <summary>
        /// Throws a <seealso cref="ServiceNotFoundException"/> if the specified services is
        /// <see langword="null"/>.
        /// </summary>
        /// <typeparam name="T">The type of the service.</typeparam>
        /// <param name="service">The service.</param>
        /// <returns>The service.</returns>
        /// <exception cref="ServiceNotFoundException">
        /// <paramref name="service"/> is <see langword="null"/>.
        /// </exception>
        public static T ThrowIfMissing<T>(this T service) where T : class
        {
            if (service == null)
            {
                var message = Invariant($"The service of type {typeof(T).Name} is missing.");
                throw new ServiceNotFoundException(message);
            }

            return service;
        }


        /// <summary>
        /// Logs a warning if the specified service is <see langword="null"/>.
        /// </summary>
        /// <typeparam name="T">The type of the service.</typeparam>
        /// <param name="service">The service.</param>
        /// <returns>The service.</returns>
        public static T WarnIfMissing<T>(this T service) where T : class
        {
            //if (service == null)
                //Logger.Warn(CultureInfo.InvariantCulture, "The service of type {0} is missing.", typeof(T).Name);

            return service;
        }

        /// <summary>
        /// Adds the specified resource dictionary to the application.
        /// </summary>
        /// <param name="resources">The resource dictionary.</param>
        public static void RegisterResources(ResourceDictionary resources)
        {
            Application.Current.Resources.MergedDictionaries.Add(resources);
        }

        /// <summary>
        /// Removes the specified resource dictionary from the application.
        /// </summary>
        /// <param name="resources">The resource dictionary.</param>
        public static void UnregisterResources(ResourceDictionary resources)
        {
            Application.Current.Resources.MergedDictionaries.Remove(resources);
        }
    }
}
