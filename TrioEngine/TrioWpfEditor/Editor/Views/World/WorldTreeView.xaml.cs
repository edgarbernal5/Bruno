using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace TrioWpfEditor.Delivery.Views.World
{
    /// <summary>
    /// Interaction logic for WorldTree.xaml
    /// </summary>
    public partial class WorldTreeView : UserControl
    {
        public WorldTreeView()
        {
            InitializeComponent();
            //https://stackoverflow.com/questions/11926954/custom-treeview-user-control-mvvm-double-click-bubbling-event-wpf
        }
    }
}
