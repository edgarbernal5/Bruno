using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace TrioWpfFramework.Windows.Docking.ViewModels
{
    public class FloatWindowViewModel : ObservableObject, IFloatWindow
    {
        public IDockPane RootPane
        {
            get { return _rootPane; }
            set { SetProperty(ref _rootPane, value); }
        }
        private IDockPane _rootPane;

        public bool IsVisible
        {
            get { return _isVisible; }
            set { SetProperty(ref _isVisible, value); }
        }
        private bool _isVisible;

        public double Left
        {
            get { return _left; }
            set { SetProperty(ref _left, value); }
        }
        private double _left = double.NaN;

        public double Top
        {
            get { return _top; }
            set { SetProperty(ref _top, value); }
        }
        private double _top = double.NaN;

        public double Width
        {
            get { return _width; }
            set { SetProperty(ref _width, value); }
        }
        private double _width = double.NaN;

        public double Height
        {
            get { return _height; }
            set { SetProperty(ref _height, value); }
        }
        private double _height = double.NaN;

        public WindowState WindowState
        {
            get { return _windowState; }
            set { SetProperty(ref _windowState, value); }
        }
        private WindowState _windowState = WindowState.Normal;
    }
}
