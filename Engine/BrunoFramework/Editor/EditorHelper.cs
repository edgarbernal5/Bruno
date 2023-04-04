
using System;
using System.Linq;
using System.Windows;
using Bruno.ServiceLocation;
using BrunoWindows;

namespace BrunoFramework.Editor
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

        public static IEditorService GetEditor(this DependencyObject element)
        {
            if (element == null)
            {
                return null;
            }

            var window = element as Window;
            if (window == null)
            {
                window = Window.GetWindow(element);
            }

            if (window != null)
            {
                return window.DataContext as IEditorService;
            }

            foreach (var ancestor in element.GetSelfAndVisualAncestors().OfType<FrameworkElement>())
            {
                var editor = ancestor.DataContext as IEditorService;
                if (editor != null)
                {
                    return editor;
                }
            }

            return null;
        }
    }
}
