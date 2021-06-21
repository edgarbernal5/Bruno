using System;
using System.ComponentModel;

namespace BrunoFramework.Editor.Game.Inspectors
{
    public abstract class EditorBase<TValue> : InspectorBase, IEditor, IDisposable
    {
        public BoundPropertyDescriptor BoundPropertyDescriptor
        { 
            get => _boundPropertyDescriptor; 
            set {
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

        public TValue Value
        {
            get
            {
                return (TValue)RawValue;
            }
            set
            {
                if (Equals(Value, value))
                    return;

                object newValue = value;
                RawValue = newValue;

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

        public EditorBase()
        {

        }

        public virtual void Dispose()
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
