
using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Threading;

namespace BrunoWindows
{
    public static class WindowsPlatform
    {
        private static bool? inDesignMode;

        private static Dispatcher _dispatcher;

        public static bool InDesignMode
        {
            get
            {
                if (inDesignMode == null)
                {
                    var descriptor = DependencyPropertyDescriptor.FromProperty(DesignerProperties.IsInDesignModeProperty, typeof(FrameworkElement));
                    inDesignMode = (bool)descriptor.Metadata.DefaultValue;
                }

                return inDesignMode.GetValueOrDefault(false);
            }
        }

        static WindowsPlatform()
        {
            _dispatcher = Dispatcher.CurrentDispatcher;
        }

        public static bool PropertyChangeNotificationsOnUIThread => true;

        private static bool CheckAccess()
        {
            return _dispatcher == null || _dispatcher.CheckAccess();
        }
        public static void OnUIThread(Action action)
        {
            if (CheckAccess())
            {
                action();
            }
            else
            {
                Exception exception = null;
                Action method = () => {
                    try
                    {
                        action();
                    }
                    catch (Exception ex)
                    {
                        exception = ex;
                    }
                };
                _dispatcher.Invoke(method);
                if (exception != null)
                    throw new System.Reflection.TargetInvocationException("An error occurred while dispatching a call to the UI Thread", exception);
            }
        }
    }
}
