using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace TrioWpfFramework.Windows.Framework
{
    public class Bootstrapper
    {
        /// <summary>
        /// The application.
        /// </summary>
        public Application Application { get; protected set; }

        /// <summary>
        /// Initializes a new instance of the <see cref="Bootstrapper"/> class.
        /// </summary>
        public Bootstrapper()
        {
            if (WpfWindowsExtensions.IsInDesignMode)
                StartDesignTime();
            else
                StartRuntime();
        }
        private void StartDesignTime()
        {
            OnConfigure();
        }

        private void StartRuntime()
        {
            // ----- Silverlight-specific initialization.
            Application = Application.Current;
            Application.Startup += OnStartup;
            Application.Exit += OnExit;

            // ----- Application-specific initialization.
            OnConfigure();
        }


        /// <summary>
        /// Configures the application.
        /// </summary>
        /// <remarks>
        /// Derived classes can override this method to perform application-specific initialization,
        /// such as setting up an IoC container, configuring services, etc.
        /// </remarks>
        protected virtual void OnConfigure()
        {
        }

        /// <summary>
        /// Called when the application is started.
        /// </summary>
        /// <param name="sender">The <see cref="Application"/>.</param>
        /// <param name="eventArgs">
        /// The <see cref="StartupEventArgs"/> instance containing the event data.
        /// </param>
        protected virtual void OnStartup(object sender, StartupEventArgs eventArgs)
        {
        }

        /// <summary>
        /// Called when an exception is thrown by an application but not handled.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="eventArgs">
        /// The <see cref="DispatcherUnhandledExceptionEventArgs"/> instance containing the event
        /// data.
        /// </param>
        protected virtual void OnUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs eventArgs)
        {
        }

        /// <summary>
        /// Called just before the application shuts down (cannot be canceled).
        /// </summary>
        /// <param name="sender">The <see cref="Application"/>.</param>
        /// <param name="eventArgs">
        /// The <see cref="ExitEventArgs"/> instance containing the event data.
        /// </param>
        protected virtual void OnExit(object sender, ExitEventArgs eventArgs)
        {
        }
    }
}
