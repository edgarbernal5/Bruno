using System;
using System.Diagnostics;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows.Framework
{
    public class Screen                 // A screen
        : ValidatableObservableObject,  // implements INotifyPropertyChanged, INotifyDataErrorInfo
          IScreen,                      // is aware of its IConductor,
          IActivatable,                 // has an activation logic,
          IGuardClose,                  // may prevent closing,
          IDisplayName                  // and has a title.
    {
        public bool IsOpen
        {
            get { return _isOpen; }
            private set { SetProperty(ref _isOpen, value); }
        }
        private bool _isOpen;

        public bool IsActive
        {
            get { return _isActive; }
            private set { SetProperty(ref _isActive, value); }
        }
        private bool _isActive;

        public IConductor Conductor
        {
            get { return _conductor; }
            set
            {
                if (_conductor == value)
                    return;

                if (_conductor != null && value != null)
                    throw new InvalidOperationException("Cannot override conductor. Screen is already being conducted.");

                _conductor = value;
                RaisePropertyChanged();
            }
        }
        private IConductor _conductor;


        /// <summary>
        /// Gets or sets the display name (title).
        /// </summary>
        /// <value>The display name (title) of the screen.</value>
        public string DisplayName
        {
            get { return _displayName; }
            set { SetProperty(ref _displayName, value); }
        }
        private string _displayName;


        /// <inheritdoc/>
        public event EventHandler<ActivationEventArgs> Activated;


        /// <inheritdoc/>
        public event EventHandler<DeactivationEventArgs> Deactivated;

        public Screen()
        {
            _displayName = GetType().FullName;
        }

        void IActivatable.OnActivate()
        {
            Debug.Assert(IsOpen || !IsActive, "Invalid state: An item can only be active when it is open.");
            if (IsActive)
                return;

            bool opened = !IsOpen;
            IsOpen = true;
            IsActive = true;
            OnActivated(new ActivationEventArgs(opened));
        }

        protected virtual void OnActivated(ActivationEventArgs eventArgs)
        {
            Activated?.Invoke(this, eventArgs);
        }

        void IActivatable.OnDeactivate(bool close)
        {
            Debug.Assert(IsOpen || !IsActive, "Invalid state: An item can only be active when it is open.");
            if (!IsOpen)
                return;

            if (!IsActive && !close)
                return;

            IsActive = false;
            if (close)
                IsOpen = false;

            OnDeactivated(new DeactivationEventArgs(close));
        }

        protected virtual void OnDeactivated(DeactivationEventArgs eventArgs)
        {
            Deactivated?.Invoke(this, eventArgs);
        }

        public virtual Task<bool> CanCloseAsync()
        {
            return Task.FromResult(true);
        }
    }
}
