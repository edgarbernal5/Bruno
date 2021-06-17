
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;

namespace BrunoWindows
{
    [DataContract]
    public class PropertyChangedBase : INotifyPropertyChangedEx
    {
        public PropertyChangedBase()
        {
            IsNotifying = true;
        }

        public virtual event PropertyChangedEventHandler PropertyChanged;

        public virtual bool IsNotifying { get; set; }

        public virtual void NotifyOfPropertyChange([CallerMemberName] string propertyName = null)
        {
            if (IsNotifying && PropertyChanged != null)
            {
                var eventArgs = new PropertyChangedEventArgs(propertyName);
                if (WindowsPlatform.PropertyChangeNotificationsOnUIThread)
                {
                    WindowsPlatform.OnUIThread(() => OnPropertyChanged(eventArgs));
                }
                else
                {
                    OnPropertyChanged(eventArgs);
                }
            }
        }

        public void NotifyOfPropertyChange<TProperty>(Expression<Func<TProperty>> property)
        {
            NotifyOfPropertyChange(property.GetMemberInfo().Name);
        }

        [EditorBrowsable(EditorBrowsableState.Never)]
        protected virtual void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            PropertyChanged?.Invoke(this, e);
        }

        public virtual bool Set<T>(ref T oldValue, T newValue, [CallerMemberName] string propertyName = null)
        {
            if (EqualityComparer<T>.Default.Equals(oldValue, newValue))
            {
                return false;
            }

            oldValue = newValue;

            NotifyOfPropertyChange(propertyName ?? string.Empty);

            return true;
        }

        public void Refresh()
        {
            NotifyOfPropertyChange(string.Empty);
        }
    }
}
