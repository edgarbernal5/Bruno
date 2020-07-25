
using System;
using System.Windows;
using Estero.ServiceLocation;

namespace EsteroFramework.Editor
{
    public static class EditorHelper
    {
        public static void RegisterView(this ServiceContainer serviceContainer, Type viewModelType, Type view)
        {
            if (serviceContainer == null)
                throw new ArgumentNullException(nameof(serviceContainer));
            if (viewModelType == null)
                throw new ArgumentNullException(nameof(viewModelType));

            serviceContainer.RegisterPerRequest(typeof(FrameworkElement), viewModelType.FullName, view);
        }

        public static void RegisterResources(ResourceDictionary resources)
        {
            Application.Current.Resources.MergedDictionaries.Add(resources);
        }
        public static void UnregisterResources(ResourceDictionary resources)
        {
            Application.Current.Resources.MergedDictionaries.Remove(resources);
        }
    }
}
