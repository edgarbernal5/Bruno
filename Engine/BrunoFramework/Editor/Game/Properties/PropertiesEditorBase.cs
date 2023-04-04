using System;
using System.ComponentModel;
using System.Globalization;
using System.Windows.Data;

namespace BrunoFramework.Editor.Game.Properties
{
    public abstract class PropertiesEditorBase<TValue> : PropertiesBase, IPropertiesEditor, IDisposable
    {
        public BoundPropertyDescriptor BoundPropertyDescriptor
        { 
            get => _boundPropertyDescriptor;
            set
            {
                CleanupPropertyChanged();

                _boundPropertyDescriptor = value;

                if (value.PropertyDescriptor.SupportsChangeEvents)
                {
                    value.ValueChanged += OnValueChanged;
                }
                else if (typeof(INotifyPropertyChanged).IsAssignableFrom(value.PropertyOwner.GetType()))
                {
                    ((INotifyPropertyChanged)value.PropertyOwner).PropertyChanged += OnPropertyChanged;
                }
            }
        }
        private BoundPropertyDescriptor _boundPropertyDescriptor;

        public IValueConverter Converter
        {
            get;
            set;
        }

        public TValue Value
        {
            get
            {
                if (!typeof(TValue).IsAssignableFrom(BoundPropertyDescriptor.PropertyDescriptor.PropertyType))
                {
                    if (Converter == null)
                        throw new InvalidCastException("No converter was specified for Inspector Editor");

                    return (TValue)Converter.Convert(RawValue, typeof(TValue), null, CultureInfo.CurrentCulture);
                }
                return (TValue)RawValue;
            }
            set
            {
                if (Equals(Value, value))
                    return;

                object newValue = value;
                if (!typeof(TValue).IsAssignableFrom(BoundPropertyDescriptor.PropertyDescriptor.PropertyType))
                {
                    if (Converter == null)
                        throw new InvalidCastException("No converter was specified for Inspector Editor");

                    newValue = Converter.ConvertBack(value, BoundPropertyDescriptor.PropertyDescriptor.PropertyType, null, CultureInfo.CurrentCulture);
                }

                IsNotifying = false;
                RawValue = newValue;
                IsNotifying = true;

                OnValueChanged();
            }
        }

        protected object RawValue
        {
            get { return BoundPropertyDescriptor.Value; }
            set { BoundPropertyDescriptor.Value = value; }
        }

        public override string Name
        {
            get { return BoundPropertyDescriptor.PropertyDescriptor.DisplayName; }
        }

        public override bool IsReadOnly
        {
            get { return BoundPropertyDescriptor.PropertyDescriptor.IsReadOnly; }
        }

        protected override void OnDisposing(bool disposing)
        {
            CleanupPropertyChanged();
        }

        private void OnValueChanged()
        {
            NotifyOfPropertyChange(() => Value);
        }

        private void OnValueChanged(object sender, EventArgs e)
        {
            OnValueChanged();
        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName.Equals(BoundPropertyDescriptor.PropertyDescriptor.Name))
                OnValueChanged();
        }

        private void CleanupPropertyChanged()
        {
            if (_boundPropertyDescriptor != null)
            {
                if (_boundPropertyDescriptor.PropertyDescriptor.SupportsChangeEvents)
                {
                    _boundPropertyDescriptor.ValueChanged -= OnValueChanged;
                }
                else if (typeof(INotifyPropertyChanged).IsAssignableFrom(_boundPropertyDescriptor.PropertyOwner.GetType()))
                {
                    ((INotifyPropertyChanged)_boundPropertyDescriptor.PropertyOwner).PropertyChanged -= OnPropertyChanged;
                }
            }
        }
    }
}
