using BrunoFramework.Editor.Units.Console;
using System.Windows;
using System.Windows.Controls;

namespace BrunoFramework.Editor.Units
{
    public partial class ConsoleView : UserControl, IConsoleView
    {
        private ConsoleViewModel m_consoleViewModel;

        public ConsoleView()
        {
            InitializeComponent();

            DataContextChanged += OnDataContextChanged;
            Unloaded += OnUnloaded;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            var consoleViewModel = DataContext as ConsoleViewModel;

            if (m_consoleViewModel != null)
            {
                m_consoleViewModel.View = null;
            }

            m_consoleViewModel = consoleViewModel;

            if (consoleViewModel != null)
            {
                m_consoleViewModel.View = this;
            }
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
        }

        public void AppendText(string text)
        {
            txtConsole.AppendText(text);
            ScrollToEnd();
        }

        public void Clear()
        {
            txtConsole.Clear();
            ScrollToEnd();
        }

        public void ScrollToEnd()
        {
            txtConsole.ScrollToEnd();
        }

        public void SetText(string text)
        {
            txtConsole.Text = text;
            ScrollToEnd();
        }
    }
}
