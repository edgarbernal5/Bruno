
using System;
using System.ComponentModel;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace BrunoWindows
{
    public static class WindowsPlatform
    {
        private static bool? g_inDesignMode;

        private static Dispatcher g_dispatcher;

        public static bool InDesignMode
        {
            get
            {
                if (g_inDesignMode == null)
                {
                    var descriptor = DependencyPropertyDescriptor.FromProperty(DesignerProperties.IsInDesignModeProperty, typeof(FrameworkElement));
                    g_inDesignMode = (bool)descriptor.Metadata.DefaultValue;
                }

                return g_inDesignMode.GetValueOrDefault(false);
            }
        }

        static WindowsPlatform()
        {
            g_dispatcher = Dispatcher.CurrentDispatcher;
        }

        public static bool PropertyChangeNotificationsOnUIThread => true;

        private static bool CheckAccess()
        {
            return g_dispatcher == null || g_dispatcher.CheckAccess();
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
                g_dispatcher.Invoke(method);
                if (exception != null)
                {
                    throw new System.Reflection.TargetInvocationException("An error occurred while dispatching a call to the UI Thread", exception);
                }
            }
        }

        public static Task OnUIThreadAsync(Func<Task> action)
        {
            return g_dispatcher.InvokeAsync(action).Task.Unwrap();
        }
    }
}
