using Estero;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;

namespace TrioWpfFramework.Windows
{
    [Serializable]
    [DataContract(IsReference = true)]
    public abstract class ObservableObject : INotifyPropertyChanged
    {
        [field: NonSerialized]
        private bool _enableUIThreadMarshaling;

        /// <summary>
        /// Gets or sets a value indicating whether the <see cref="PropertyChanged"/> event is
        /// raised on the UI thread.
        /// </summary>
        /// <value>
        /// <see langword="true"/> if the <see cref="PropertyChanged"/> event is raised
        /// asynchronously on the UI thread; otherwise, <see langword="false"/> if the
        /// <see cref="PropertyChanged"/> event is raised synchronously on the current thread. The
        /// default value is <see langword="false"/>.
        /// </value>
        /// <remarks>
        /// Changes of a view model that affect the view should be performed on the UI thread.
        /// Therefore UI thread marshaling is usually not necessary and disabled by default.
        /// </remarks>
        protected bool EnableUIThreadMarshaling
        {
            get { return _enableUIThreadMarshaling; }
            set { _enableUIThreadMarshaling = value; }
        }

        [field: NonSerialized]
        /// <summary>
        /// Raised when a property on this object has a new value.
        /// </summary>
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Sets the value of the property and raises the <see cref="PropertyChanged"/> event. (Does
        /// nothing if the new value matches the current property value.)
        /// </summary>
        protected virtual bool SetProperty<T>(ref T field, T value, [CallerMemberName] string propertyName = null)
        {
            if (EqualityComparer<T>.Default.Equals(field, value))
                return false;

            field = value;
            OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
            return true;
        }

        protected virtual void OnPropertyChanged(PropertyChangedEventArgs eventArgs)
        {
            if (eventArgs == null)
                throw new ArgumentNullException(nameof(eventArgs));

            var handler = PropertyChanged;
            if (handler != null)
            {
                if (EnableUIThreadMarshaling)
                {
                    // Ensure that the changed notifications are raised on the UI thread.
                    WpfWindowsExtensions.CheckBeginInvokeOnUI(() => handler(this, eventArgs));
                }
                else
                {
                    handler(this, eventArgs);
                }
            }
        }

        protected void RaisePropertyChanged<T>(Expression<Func<T>> expression)
        {
#if DEBUG
            // (Only in DEBUG to avoid double check in RELEASE.)
            if (expression == null)
                throw new ArgumentNullException(nameof(expression), "expression cannot be null. Call RaisePropertyChanged() instead, if you want to indicate that all properties have changed.");
#endif

            string propertyName = ObjectHelper.GetPropertyName(expression);
            OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
        }

        protected void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
        }
    }
}
