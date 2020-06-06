
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;

namespace EsteroWindows
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
                if (WindowsPlatform.PropertyChangeNotificationsOnUIThread)
                {
                    OnUIThread(() => OnPropertyChanged(new PropertyChangedEventArgs(propertyName)));
                }
                else
                {
                    OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
                }
            }
        }

        public void NotifyOfPropertyChange<TProperty>(Expression<Func<TProperty>> property)
        {
            NotifyOfPropertyChange(property.GetMemberInfo().Name);
        }

        [EditorBrowsable(EditorBrowsableState.Never)]
        protected void OnPropertyChanged(PropertyChangedEventArgs e)
        {
            PropertyChanged?.Invoke(this, e);
        }

        protected virtual void OnUIThread(Action action)
        {
            WindowsPlatform.OnUIThread(action);
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
