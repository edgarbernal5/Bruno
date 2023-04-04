
using System.Windows;
using System.Windows.Controls;

namespace BrunoFramework.Editor.Game
{
    public partial class WorldOutlineView : UserControl, IWorldOutlineView
    {
        private WorldOutlineViewModel m_worldOutlineView;
        public WorldOutlineView()
        {
            InitializeComponent();

            DataContextChanged += OnDataContextChanged;
        }

        private void OnDataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            var worldOutlineViewModel = DataContext as WorldOutlineViewModel;

            if (m_worldOutlineView != null)
            {
                m_worldOutlineView.View = null;
            }

            m_worldOutlineView = worldOutlineViewModel;

            if (worldOutlineViewModel != null)
            {
                m_worldOutlineView.View = this;
            }
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
        }

        public string SearchText => txtFind.Text;
    }
}
