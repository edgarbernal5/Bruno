using Bruno.Collections;
using BrunoFramework.Editor.Game.Properties;
using System;

namespace BrunoFramework.Editor.Units
{
    public class PropertiesUnit : EditorUnit, IPropertiesService
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;
        public PropertiesViewModel ViewModel => m_propertiesViewModel;

        public IPropertiesObject SelectedObject 
        {
            get => m_propertiesViewModel.SelectedObject;
            set => m_propertiesViewModel.SelectedObject = value;
        }

        private PropertiesViewModel m_propertiesViewModel;

        public PropertiesUnit()
        {
        }

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IPropertiesService), null, this);
            m_propertiesViewModel = new PropertiesViewModel(Editor);
        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("OpenProperties", new DelegateCommand(() => OpenView()))
                {
                    Text = "Properties",
                }
            );
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("WindowsGroup", "Windows"),
                    new TreeNode<ICommandItem>(CommandItems["OpenProperties"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IPropertiesService));
        }

        protected override void OnShutdown()
        {
            m_propertiesViewModel = null;
        }

        public override EditorDockTabViewModel GetDockTabViewModel(string dockId)
        {
            if (dockId == PropertiesViewModel.DockIdString)
            {
                return m_propertiesViewModel;
            }

            return base.GetDockTabViewModel(dockId);
        }

        private void OpenView()
        {
            Editor.ActivateItem(m_propertiesViewModel);
        }
    }
}
