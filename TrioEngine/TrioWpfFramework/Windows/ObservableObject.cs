using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace TrioWpfFramework.Windows
{
    public abstract class ObservableObject : INotifyPropertyChanged
    {
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
                //if (EnableUIThreadMarshaling)
                //{
                //    // Ensure that the changed notifications are raised on the UI thread.
                //    WindowsHelper.CheckBeginInvokeOnUI(() => handler(this, eventArgs));
                //}
                //else
                //{
                //    handler(this, eventArgs);
                //}
                handler(this, eventArgs);
            }
        }

        protected void RaisePropertyChanged([CallerMemberName] string propertyName = null)
        {
            OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
        }
    }
}
