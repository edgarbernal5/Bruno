using BrunoWindows;
using System;
using System.Windows;

namespace BrunoFramework
{
    public class BootstrapperBase
    {
        private bool _isInitialized;
        protected Application Application { get; set; }

        public void Initialize()
        {
            if (_isInitialized)
            {
                return;
            }

            _isInitialized = true;

            if (WindowsPlatform.InDesignMode)
            {
                try
                {
                    StartDesignTime();
                }
                catch
                {
                    _isInitialized = false;
                    throw;
                }
            }
            else
            {
                StartRuntime();
            }
        }

        protected virtual void StartDesignTime()
        {
            OnConfigure();
        }

        protected virtual void StartRuntime()
        {
            Application = Application.Current;
            Application.Startup += OnStartup;
            Application.Exit += OnExit;

            OnConfigure();
        }

        protected virtual void OnExit(object sender, ExitEventArgs eventArgs)
        {
        }

        protected virtual void OnStartup(object sender, StartupEventArgs eventArgs)
        {
        }

        protected virtual void OnConfigure()
        {
        }
    }
}
