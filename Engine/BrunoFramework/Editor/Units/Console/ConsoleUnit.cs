
using Bruno.Collections;
using Bruno.Logging;
using BrunoWindows;
using System.Linq;
using System.Text;

namespace BrunoFramework.Editor.Units
{
    public class ConsoleUnit : EditorUnit, IConsoleService
    {
        private TreeNodeCollection<ICommandItem> _menuNodes;
        private ConsoleViewModel m_consoleViewModel;
        private StringBuilder m_stringBuilder;
        private InMemoryLog m_inMemoryLog;

        protected override void OnInitialize()
        {
            Editor.Services.RegisterInstance(typeof(IConsoleService), this);

            m_stringBuilder = new StringBuilder();
            m_consoleViewModel = new ConsoleViewModel(Editor);

            m_inMemoryLog = Logger.GetLog().Logs.OfType<InMemoryLog>().FirstOrDefault();
            if (m_inMemoryLog != null)
            {
                m_inMemoryLog.MessageWritten += OnMessageWritten;
            }
        }

        protected override void OnStartup()
        {
            AddCommands();
            AddMenus();

            m_inMemoryLog.FillWrittenLog(m_stringBuilder);
        }

        private void AddCommands()
        {
            CommandItems.Add(
                new CommandItem("OpenConsole", new DelegateCommand(() => OpenView()))
                {
                    Text = "Console",
                }
            );
        }

        private void AddMenus()
        {
            _menuNodes = new TreeNodeCollection<ICommandItem>()
            {
                new TreeNode<ICommandItem>(new CommandGroup("WindowsGroup", "Windows"),
                    new TreeNode<ICommandItem>(CommandItems["OpenConsole"]))
            };

            Editor.MenuNodes.Add(_menuNodes);
        }

        protected override void OnUninitialize()
        {
            Editor.Services.UnregisterHandler(typeof(IConsoleService));
        }

        protected override void OnShutdown()
        {
            RemoveMenus();
            RemoveCommands();
            m_consoleViewModel = null;
        }

        private void RemoveMenus()
        {
            Editor.MenuNodes.Remove(_menuNodes);
            _menuNodes.Clear();
        }

        private void RemoveCommands()
        {
            CommandItems.Clear();
        }

        public override EditorDockTabViewModel GetDockTabViewModel(string dockId)
        {
            if (dockId == ConsoleViewModel.DockIdString)
            {
                return m_consoleViewModel;
            }

            return base.GetDockTabViewModel(dockId);
        }

        private void OpenView()
        {
            Editor.ActivateItem(m_consoleViewModel);
        }

        private void OnMessageWritten(string message)
        {
            AppendLine(message);
        }

        public void AppendLine(string message)
        {
            //m_stringBuilder.AppendLine(message);
            if (m_consoleViewModel == null || m_consoleViewModel.View == null) return;

            WindowsPlatform.OnUIThread(() =>
            {
                m_consoleViewModel.View.SetText(m_stringBuilder.ToString());
            });
        }

        public void Append(string message)
        {
            m_stringBuilder.Append(message);

            if (m_consoleViewModel == null || m_consoleViewModel.View == null) return;

            WindowsPlatform.OnUIThread(() =>
            {
                m_consoleViewModel.View.SetText( m_stringBuilder.ToString());
            });
        }

        public void Clear()
        {
            m_stringBuilder.Clear();

            if (m_consoleViewModel == null || m_consoleViewModel.View == null) return;

            WindowsPlatform.OnUIThread(() =>
            {
                m_consoleViewModel.View.Clear();
            });
        }

        public void Flush()
        {
            WindowsPlatform.OnUIThread(() =>
            {
                m_consoleViewModel.View.SetText(m_stringBuilder.ToString());
            });
        }
    }
}
