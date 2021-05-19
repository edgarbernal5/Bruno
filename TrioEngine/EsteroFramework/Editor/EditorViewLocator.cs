
using Estero.ServiceLocation;
using System;
using System.Windows;

namespace EsteroFramework.Editor
{
    public class EditorViewLocator : IViewLocator
    {
        private readonly ServiceContainer _services;

        public EditorViewLocator(ServiceContainer services)
        {
            if (services == null)
                throw new ArgumentNullException(nameof(services));

            _services = services;
        }

        public FrameworkElement GetView(object viewModel, DependencyObject parent = null, object context = null)
        {
            if (viewModel == null)
                throw new ArgumentNullException(nameof(viewModel));

            var type = viewModel.GetType();
            while (type != null)
            {
                var view = (FrameworkElement)_services.GetInstance(typeof(FrameworkElement), type.FullName);
                if (view != null)
                    return view;

                type = type.BaseType;
            }

            throw new EditorException(string.Format("Unable to resolve view for view model of type {0}", viewModel.GetType()));
        }
    }
}
