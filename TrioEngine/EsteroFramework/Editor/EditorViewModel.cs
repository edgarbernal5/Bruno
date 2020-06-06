using Estero.ServiceLocation;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace EsteroFramework.Editor
{
    public class EditorViewModel : Screen, IEditorService
    {
        public ServiceContainer Services { get; private set; }

        public EditorViewModel(ServiceContainer serviceContainer)
        {
            Services = serviceContainer;
        }

        public void Configure()
        {
            DisplayName = "Estero Editor";

            Services.RegisterInstance(typeof(IEditorService), null, this);
            Services.RegisterPerRequest(typeof(IViewLocator), null, typeof(EditorViewLocator));
            Services.RegisterView(typeof(EditorViewModel), typeof(EditorWindow));
        }

        public void Startup()
        {

        }

    }
}
