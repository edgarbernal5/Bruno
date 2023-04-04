
using BrunoFramework.Editor.Units.Console;
using System;
using System.Windows.Input;

namespace BrunoFramework.Editor.Units
{
    public class ConsoleViewModel : EditorDockTabViewModel
    {
        internal const string DockIdString = "Console";
        private readonly IConsoleService m_consoleService;

        public IConsoleView View
        {
            get => m_view;
            set
            {
                m_view = value;
                if (m_view != null)
                    m_consoleService.Flush();
            }
        }
        private IConsoleView m_view;

        public ICommand ClearCommand
        {
            get
            {
                if (m_clearCommand == null)
                {
                    m_clearCommand = new DelegateCommand(() =>
                      {
                          m_consoleService.Clear();
                      });
                }
                return m_clearCommand;
            }
        }
        private ICommand m_clearCommand;

        public ConsoleViewModel(IEditorService editor)
            : base()
        {
            DisplayName = "Console";
            DockId = DockIdString;

            m_consoleService = editor.Services.GetInstance<IConsoleService>();
        }
    }
}
