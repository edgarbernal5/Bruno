﻿using EsteroWindows;
using System;
using System.Threading;
using System.Threading.Tasks;

namespace EsteroFramework
{
    public class Screen : PropertyChangedBase,
        IHaveDisplayName, IActivate, IDeactivate, IGuardClose
    {
        public Screen()
        {
            _displayName = GetType().FullName;
        }

        public virtual string DisplayName
        {
            get => _displayName;
            set
            {
                _displayName = value;
                NotifyOfPropertyChange();
            }
        }
        private string _displayName;

        public virtual bool IsActive
        {
            get => _isActive;
            private set
            {
                _isActive = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isActive;

        public virtual bool IsInitialized
        {
            get => _isInitialized;
            private set
            {
                _isInitialized = value;
                NotifyOfPropertyChange();
            }
        }
        private bool _isInitialized;


        public event EventHandler<ActivationEventArgs> Activated;
        public event EventHandler<DeactivationEventArgs> AttemptingDeactivation;
        public event EventHandler<DeactivationEventArgs> Deactivated;

        void IActivate.Activate()
        {
            if (IsActive)
                return;

            var initialized = false;

            if (!IsInitialized)
            {
                //OnInitializeAsync(cancellationToken);
                IsInitialized = initialized = true;
            }

            //Log.Info("Activating {0}.", this);
            //await OnActivateAsync(cancellationToken);
            IsActive = true;

            Activated?.Invoke(this, new ActivationEventArgs
            {
                WasInitialized = initialized
            });
        }

        void IDeactivate.Deactivate(bool close)
        {
            if (IsActive || IsInitialized && close)
            {
                AttemptingDeactivation?.Invoke(this, new DeactivationEventArgs
                {
                    WasClosed = close
                });

                OnDeactivate(close);
                IsActive = false;

                Deactivated?.Invoke(this, new DeactivationEventArgs
                {
                    WasClosed = close
                });

                if (close)
                {
                    //Views.Clear();
                    //Log.Info("Closed {0}.", this);
                }
            }
        }

        protected virtual void OnDeactivate(bool close)
        {
        }

        public Task<bool> CanCloseAsync(CancellationToken cancellationToken = default)
        {
            return Task.FromResult(true);
        }
    }
}