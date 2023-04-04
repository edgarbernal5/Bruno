
using Bruno.Logging;
using BrunoWindows;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace BrunoFramework
{
    public class Screen : PropertyChangedBase, IScreen
    {
        private static readonly ILog Logger = Bruno.Logging.Logger.GetLog();

        public Screen() 
            : base()
        {
            m_displayName = GetType().FullName;
        }

        public virtual string DisplayName
        {
            get => m_displayName;
            set
            {
                m_displayName = value;
                NotifyOfPropertyChange();
            }
        }
        private string m_displayName;

        public virtual bool IsActive
        {
            get => m_isActive;
            private set
            {
                m_isActive = value;
                NotifyOfPropertyChange();
            }
        }
        private bool m_isActive;

        public virtual bool IsInitialized
        {
            get => m_isInitialized;
            private set
            {
                m_isInitialized = value;
                NotifyOfPropertyChange();
            }
        }
        private bool m_isInitialized;

        public IConductor Conductor
        {
            get => m_conductor;
            set
            {
                m_conductor = value;
                NotifyOfPropertyChange();
            }
        }
        private IConductor m_conductor;

        public event EventHandler<ActivationEventArgs> Activated;
        public event EventHandler<DeactivationEventArgs> Deactivating;
        public event EventHandler<DeactivationEventArgs> Deactivated;

        async Task IActivate.ActivateAsync(CancellationToken cancellationToken)
        {
            if (IsActive)
                return;

            var initialized = false;

            if (!IsInitialized)
            {
                await OnInitializeAsync(cancellationToken);
                IsInitialized = initialized = true;
            }

            //Logger.Info("Activating {0}.", this);
            await OnActivateAsync(cancellationToken);
            IsActive = true;

            Activated?.Invoke(this, new ActivationEventArgs
            {
                WasInitialized = initialized
            });
        }

        async Task IDeactivate.DeactivateAsync(bool close, CancellationToken cancellationToken)
        {
            if (IsActive || (IsInitialized && close))
            {
                Deactivating?.Invoke(this, new DeactivationEventArgs
                {
                    WasClosed = close
                });

                //Logger.Info("Deactivating {0}.", this);
                await OnDeactivateAsync(close, cancellationToken);
                IsActive = false;

                Deactivated?.Invoke(this, new DeactivationEventArgs
                {
                    WasClosed = close
                });

                if (close)
                {
                    //Views.Clear();
                    Logger.Info("Closed {0}.", this);
                }
            }
        }

        protected virtual Task OnDeactivateAsync(bool close, CancellationToken cancellationToken)
        {
            return Task.FromResult(true);
        }

        protected virtual Task OnInitializeAsync(CancellationToken cancellationToken)
        {
            return Task.FromResult(true);
        }

        protected virtual Task OnActivateAsync(CancellationToken cancellationToken)
        {
            return Task.FromResult(true);
        }

        public virtual Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            return Task.FromResult(true);
        }

        public async Task TryCloseAsync(bool? dialogResult = null)
        {
            await m_conductor.DeactivateItemAsync(this, true, CancellationToken.None);

            /*var closeMethod = GetType().GetMethod("Close", new Type[0]);
            if (closeMethod != null)
            {
                var isClosed = false;
                if (dialogResult != null)
                {
                    var resultProperty = GetType().GetProperty("DialogResult");
                    if (resultProperty != null)
                    {
                        resultProperty.SetValue(this, dialogResult, null);
                        isClosed = true;
                    }
                }
            }*/
            //Close()??
            //WindowsPlatform.OnUIThreadAsync
        }
    }
}
