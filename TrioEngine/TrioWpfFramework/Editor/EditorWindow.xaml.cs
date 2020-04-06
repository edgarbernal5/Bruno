
using System.Windows;

namespace TrioWpfFramework
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class EditorWindow : Window
    {
        public EditorWindow()
        {
            InitializeComponent();

            //GraphicsDeviceService.Service.DeviceCreated += Service_DeviceCreated;
        }

    //    private void Service_DeviceCreated(object sender, EventArgs e)
    //    {
    //        var device = GraphicsDeviceService.Service.GraphicsDevice;
    //        var ibuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits, 36, ResourceUsage.Immutable);

    //        uint[] indices = new uint[]{
    //                // front face
    //                0, 1, 2,
    //                0, 2, 3,

    //                // back face
    //                4, 6, 5,
    //                4, 7, 6,

    //                // left face
    //                4, 5, 1,
    //                4, 1, 0,

    //                // right face
    //                3, 2, 6,
    //                3, 6, 7,

    //                // top face
    //                1, 5, 6,
    //                1, 6, 2,

    //                // bottom face
    //                4, 0, 3,
    //                4, 3, 7
    //            };

    //        ibuffer.SetData<uint>(indices);
    //        Effect effect = new Effect(device, @"Effects\GizmoModel.fx");

    //        Scene scene = new Scene();

    //        GameObject gameObject = GameObject.Create("GO1");
    //        string name = gameObject.Name;
    //        int id = gameObject.Id;

    //        BuildCoordinatorSettings settings = new BuildCoordinatorSettings();
    //        //settings.IntermediateDirectory = "";
    //        //settings.OutputDirectory = "";
    //        //settings.RootDirectory = "";

    //        BuildCoordinator buildCoordinator = new BuildCoordinator(settings);
    //    }
    }
}
